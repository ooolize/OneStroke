/*
 * @Description:
 * @Author: lize
 * @Date: 2024-09-24
 * @LastEditors: lize
 */
#pragma once

#include <queue>
#include <thread>

#include "interface/handle.h"
#include "netevent.h"
namespace lz {
namespace ZhouBoTong {
class Schedule {
 public:
  Schedule();
  ~Schedule() noexcept = default;
  HandleID add_task(HandleInfo handle);
  void loop();
  void remove_task(HandleID id);

 private:
  Schedule(const Schedule&) = delete;
  Schedule& operator=(const Schedule&) = delete;
  Schedule(Schedule&&) = delete;
  Schedule& operator=(Schedule&&) = delete;

 private:
  std::jthread _jthread;
  NetEvent _net_event{};
  std::queue<HandleInfo> _ready_queue{};
  std::queue<HandleInfo> _wait_queue{};
  std::queue<HandleID> _cancel_queue{};
};

class GetSchedule {
 public:
  static Schedule& get_instance() {
    static Schedule schedule;
    return schedule;
  }
};
}  // namespace ZhouBoTong
}  // namespace lz