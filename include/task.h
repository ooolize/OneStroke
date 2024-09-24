/*
 * @Description:
 * @Author: lize
 * @Date: 2024-09-24
 * @LastEditors: lize
 */
#pragma once

#include <coroutine>

#include "schedule.h"
namespace lz {
namespace ZhouBoTong {
class CoRoutineHandler : public Handle {
 public:
  CoRoutineHandler(std::coroutine_handle<> coroutine_handle)
    : _coroutine_handle(coroutine_handle) {
  }
  void run() {
    _coroutine_handle.resume();
  }

 private:
  std::coroutine_handle<> _coroutine_handle;
};

class Promise {
 public:
  Promise() = default;
  auto initial_suspend() {
    return std::suspend_never{};
  }
  auto final_suspend() noexcept {
    return std::suspend_never{};
  }
  auto get_return_object() {
    return std::coroutine_handle<Promise>::from_promise(*this);
  }
  auto unhandled_exception() {
  }
  int return_value(int val) {
    _value = val;
    return val;
  };
  int get_return_value() {
    return _value;
  }
  int _value;
};

class Task {
 public:
  using promise_type = Promise;
  Task(std::coroutine_handle<promise_type> coroutine_handle)
    : _coroutine_handle(coroutine_handle) {
  }
  ~Task() {
    if (_coroutine_handle) {
      _coroutine_handle.destroy();
    }
  }
  auto get_return_value() {
    return _coroutine_handle.promise().get_return_value();
  }
  bool await_ready() {
    return false;
  }
  void await_suspend(std::coroutine_handle<promise_type> coroutine_handle) {
    HandleInfo handle_info{
      .id = 0,
      .handle = new CoRoutineHandler(coroutine_handle),
    };
    GetSchedule::get_instance().add_task(handle_info);
  }
  // shedule调用完成后 会调用这个函数
  int await_resume() {
    return _coroutine_handle.promise().get_return_value();
  }

 private:
  std::coroutine_handle<promise_type> _coroutine_handle;
};
}  // namespace ZhouBoTong
}  // namespace lz