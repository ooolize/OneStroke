/*
 * @Description:
 * @Author: lize
 * @Date: 2024-09-25
 * @LastEditors: lize
 */

#pragma once

#include <fmt/core.h>

#include <chrono>
#include <coroutine>

#include "schedule.h"

namespace lz {
namespace ZhouBoTong {

class SleepAwaiter {
 public:
  explicit SleepAwaiter(std::chrono::seconds seconds) : _seconds(seconds) {
  }
  bool await_ready() {
    return false;
  }
  void await_suspend(std::coroutine_handle<> handle) {
    // _caller_handle = handle;
    HandleInfo handle_info{
      .id = getNextId(),
      .handle = new CoRoutineHandler(handle),
    };
    GetSchedule::get_instance().schedule_after(handle_info, _seconds);
  }
  void await_resume() {
    std::cout << "resume\n";
  }

 private:
  std::chrono::seconds _seconds{};
  //   std::coroutine_handle<> _caller_handle;
};

Task<void> co_sleep(std::chrono::seconds seconds) {
  co_await SleepAwaiter{seconds};
}

}  // namespace ZhouBoTong
}  // namespace lz
