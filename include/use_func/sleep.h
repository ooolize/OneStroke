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
#include "task.h"

namespace lz {
namespace ZhouBoTong {

class SleepAwaiter {
 public:
  explicit SleepAwaiter(std::chrono::seconds seconds) : _seconds(seconds) {
  }
  bool await_ready() {
    return false;
  }
  template <typename Promise>
  void await_suspend(std::coroutine_handle<Promise> handle) {
    HandleInfo handle_info(handle.promise().get_handle_id(),
                           std::make_unique<CoRoutineHandler>(handle));
    GetSchedule::get_instance().schedule_after(std::move(handle_info),
                                               _seconds);
  }
  void await_resume() {
    std::cout << "sleep_resume\n";
  }

 private:
  std::chrono::seconds _seconds{};
  //   std::coroutine_handle<> _caller_handle;
};

Task<void> detail_co_sleep(std::chrono::seconds seconds) {
  co_await SleepAwaiter{seconds};
}

// 不是协程
auto co_sleep(std::chrono::seconds seconds) {
  return detail_co_sleep(seconds);
}

}  // namespace ZhouBoTong
}  // namespace lz
