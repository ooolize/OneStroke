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
  _jthread = std::jthread([this]() { loop(); });
}

HandleID Schedule::add_task(HandleInfo handle) {
  _ready_queue.push(handle);
  return handle.id;
}

void Schedule::remove_task(HandleID id) {
  _cancel_queue.push(id);
}

void Schedule::loop() {
  while (1) {
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    // 如果epoll有就绪的任务，就将其放入ready队列
    // 如果wait有就绪的任务，就将其放入ready队列
    while (!_wait_queue.empty()) {
      auto handle = _wait_queue.front();
      if (handle.state == HandleState::kWait) {
        handle.state = HandleState::kReady;
        _ready_queue.push(handle);
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