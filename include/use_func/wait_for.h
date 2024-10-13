/*
 * @Description:
 * @Author: lize
 * @Date: 2024-10-11
 * @LastEditors: lize
 */
#pragma once
#include <fmt/core.h>

#include <chrono>
#include <coroutine>
#include <memory>

#include "interface/handle.h"
#include "schedule.h"
#include "sleep.h"
#include "task.h"

namespace lz {
namespace ZhouBoTong {
class TimerHandler : public Handle {
 public:
  TimerHandler(HandleID task_id, std::coroutine_handle<> coroutine_handle)
    : _handle_id(getNextId()),
      _task_id(task_id),
      _coroutine_handle(coroutine_handle) {
  }
  void run() final {
    if (_coroutine_handle.done()) {
      fmt::print("coroutine done\n");
      return;
    }
    fmt::print("{}\n", "timer done");
    //  有可能撤销先于ready/wait 就可能导致cancel队列越来越多？
    GetSchedule::get_instance().remove_task(_task_id);
    // 恢复父协程
    _coroutine_handle.resume();
  }
  HandleID get_handle_id() const final {
    return _handle_id;
  }

 private:
  // std::chrono::seconds _seconds{};
  HandleID _handle_id{};
  HandleID _task_id{};
  std::coroutine_handle<> _coroutine_handle{};
};

template <typename Task>
class WaitAwaiter {
 public:
  template <typename Fut>
  WaitAwaiter(Fut&& task, std::chrono::seconds seconds)
    : _seconds(seconds), _task(std::forward<Fut>(task)) {
  }
  bool await_ready() {
    return false;
  }
  template <typename Promise>
  void await_suspend(std::coroutine_handle<Promise> handle) {
    // 执行task
    auto t = shedule_task(std::forward<Task>(_task));

    // 开启定时 超时后中断任务协程 并且恢复父协程
    auto task_id = _task.get_handle_id();
    std::unique_ptr<TimerHandler> timer_handle =
      std::make_unique<TimerHandler>(task_id, handle);
    HandleInfo handle_info(timer_handle->get_handle_id(),
                           std::move(timer_handle));

    GetSchedule::get_instance().schedule_after(std::move(handle_info),
                                               _seconds);
  }
  void await_resume() {
    fmt::print("{}\n", "wait_resume");
  }

 private:
  std::chrono::seconds _seconds{};
  Task _task{};
};

template <typename T>
WaitAwaiter<T> co_wait_for(T&& task, std::chrono::seconds seconds) {
  return WaitAwaiter(std::forward<T>(task), seconds);
};

template <typename T>
WaitAwaiter(T&&, std::chrono::seconds) -> WaitAwaiter<T>;

Task<void> wait_func(WaitForInit wait_for_init) {
  co_await co_sleep(std::chrono::seconds(5));
  // do some thing
  fmt::print("{}\n", "wait_func");
};

// wait_func      shedule_task
// timer          

// 第一次run  恢复wait_func所以增加sleep协程 并 又co_await task


// co_await task await_suspend
// final_suspend
}  // namespace ZhouBoTong
}  // namespace lz