/*
 * @Description:
 * @Author: lize
 * @Date: 2024-09-24
 * @LastEditors: lize
 */
#include "schedule.h"

namespace lz {
namespace ZhouBoTong {

Schedule::Schedule() {
}

HandleID Schedule::schedule_now(HandleInfo handle) {
  _ready_queue.push(handle);
  return handle.id;
}

HandleID Schedule::schedule_at(HandleInfo handle, TimePoint time_point) {
  handle.state = HandleState::kWait;
  _wait_queue.push(std::pair(handle, time_point));
  return handle.id;
}

HandleID Schedule::schedule_after(HandleInfo handle, Duration duration) {
  TimePoint time_point = std::chrono::steady_clock::now() + duration;
  return schedule_at(handle, time_point);
}

void Schedule::remove_task(HandleID id) {
  _cancel_queue.push(id);
}

void Schedule::Start() {
  _jthread = std::jthread(&Schedule::loop, this);
}

void Schedule::loop() {
  while (1) {
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    // 如果epoll有就绪的任务，就将其放入ready队列
    // 如果wait有就绪的任务，就将其放入ready队列
    while (!_wait_queue.empty()) {
      auto handle = _wait_queue.front();
      if (handle.first.state == HandleState::kWait &&
          std::chrono::steady_clock::now() >= handle.second) {
        handle.first.state = HandleState::kReady;
        _ready_queue.push(handle.first);
        _wait_queue.pop();
      }
    }
    // 检查ready队列，排除掉cancel队列中的任务，否则执行
    while (!_ready_queue.empty()) {
      auto handle = _ready_queue.front();
      if (handle.state == HandleState::kCancel) {
        _ready_queue.pop();
      } else {
        handle.handle->run();
        _ready_queue.pop();
      }
    }
  }
}

}  // namespace ZhouBoTong
}  // namespace lz