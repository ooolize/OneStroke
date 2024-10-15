/*
 * @Description:
 * @Author: lize
 * @Date: 2024-09-24
 * @LastEditors: lize
 */
#pragma once

#include <map>
#include <mutex>
#include <queue>
#include <thread>

#include "interface/handle.h"
#include "utils/use_epoll.h"
namespace lz {
namespace OneStroke {
class Schedule {
 public:
  Schedule();
  ~Schedule() noexcept = default;

  void loop();
  void Start();
  void remove_task(HandleID id);
  bool is_empty();

  HandleID schedule_now(HandleInfo handle);
  HandleID schedule_at(HandleInfo handle, TimePoint time_point);
  HandleID schedule_after(HandleInfo handle, Duration duration);

 private:
  Schedule(const Schedule&) = delete;
  Schedule& operator=(const Schedule&) = delete;
  Schedule(Schedule&&) = delete;
  Schedule& operator=(Schedule&&) = delete;

 private:
  EpollEvent _epoll_event{};

  std::mutex _mutex{};
  std::jthread _jthread;
  std::queue<HandleInfo> _ready_queue{};
  std::queue<std::pair<HandleInfo, TimePoint>>
    _wait_queue{};  // 按时间有序考虑红黑树
  std::vector<HandleID> _cancel_queue{};
};

class GetSchedule {
 public:
  static Schedule& get_instance() {
    static Schedule schedule;
    return schedule;
  }
};
}  // namespace OneStroke
}  // namespace lz