/*
 * @Description:
 * @Author: lize
 * @Date: 2024-09-24
 * @LastEditors: lize
 */
#include "schedule.h"

#include <algorithm>
#include <map>
#include <ranges>

#include "use_func/echo.h"
namespace lz {
namespace ZhouBoTong {

Schedule::Schedule() {
  _cancel_queue.reserve(100);
}

HandleID Schedule::schedule_now(HandleInfo handle) {
  std::lock_guard<std::mutex> lock(_mutex);

  _ready_queue.push(std::move(handle));
  return handle.id;
}

HandleID Schedule::schedule_at(HandleInfo handle, TimePoint time_point) {
  std::lock_guard<std::mutex> lock(_mutex);

  handle.state = HandleState::kWait;
  _wait_queue.push(std::pair(std::move(handle), time_point));
  return handle.id;
}

HandleID Schedule::schedule_after(HandleInfo handle, Duration duration) {
  TimePoint time_point = std::chrono::steady_clock::now() + duration;
  return schedule_at(std::move(handle), time_point);
}

void Schedule::remove_task(HandleID id) {
  std::lock_guard<std::mutex> lock(_mutex);

  _cancel_queue.push_back(id);
}

bool Schedule::is_empty() {
  std::lock_guard<std::mutex> lock(_mutex);
  return _ready_queue.empty() && _wait_queue.empty();
}

void Schedule::Start() {
  _epoll_event.init(8082);
  _jthread = std::jthread(&Schedule::loop, this);
}

void Schedule::loop() {
  std::map<SocketID, HandleInfo> socket2handle{};

  while (1) {
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    _epoll_event.clear();
    // 如果epoll有就绪的任务，就将其放入ready队列

    _epoll_event.wait_event(-1);
    decltype(auto) ready_fd = _epoll_event.get_ready_fd();
    decltype(auto) new_fd = _epoll_event.get_new_fd();
    // 可能是新的连接
    for (const auto& fd : new_fd) {
      auto task = echo(fd);
      HandleInfo handle_info(
        fd, std::make_unique<CoRoutineHandler>(task.get_handle()));
      socket2handle[fd] = std::move(handle_info);
    }
    // 也可能是旧的连接 发消息了
    for (const auto& fd : ready_fd) {
      auto iter = socket2handle.find(fd);
      if (iter != socket2handle.end()) {
        auto handle = std::move(iter->second);
        _ready_queue.push(std::move(handle));
        socket2handle.erase(iter);
      }
    }

    // 如果wait有就绪的任务，就将其放入ready队列
    while (!_wait_queue.empty()) {
      std::lock_guard<std::mutex> lock(_mutex);
      auto state = _wait_queue.front().first.state;
      auto is_ready =
        std::chrono::steady_clock::now() >= _wait_queue.front().second;
      if (state == HandleState::kWait && is_ready) {
        auto handle = std::move(_wait_queue.front());
        handle.first.state = HandleState::kReady;
        _ready_queue.push(std::move(handle.first));
        _wait_queue.pop();
      }
    }
    // 检查ready队列，排除掉cancel队列中的任务，否则执行
    while (!_ready_queue.empty()) {
      auto handle = std::move(_ready_queue.front());
      decltype(_cancel_queue)::iterator iter;
      if (!_cancel_queue.empty() &&
          (iter = std::ranges::find(_cancel_queue, handle.id)) !=
            _cancel_queue.end()) {
        std::lock_guard<std::mutex> lock(_mutex);
        _ready_queue.pop();
        _cancel_queue.erase(iter);
        // *iter = -1;
      } else {
        handle.handle->run();
        {
          std::lock_guard<std::mutex> lock(_mutex);
          _ready_queue.pop();
        }
      }
    }
  }
}

}  // namespace ZhouBoTong
}  // namespace lz