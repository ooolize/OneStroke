/*
 * @Description:
 * @Author: lize
 * @Date: 2024-09-24
 * @LastEditors: lize
 */
#pragma once

#include <coroutine>

#include "interface/handle.h"
#include "schedule.h"
namespace lz {
namespace ZhouBoTong {

class Task {
 public:
  class Promise {
   public:
    // Promise() = default;
    // 协程首次挂起前不会挂起，继续执行
    std::suspend_never initial_suspend() {
      return {};
    }

    // 协程完成时挂起 final_suspend，保持协程状态
    std::suspend_always final_suspend() noexcept {
      return {};
    }

    Task get_return_object() {
      return Task{std::coroutine_handle<Promise>::from_promise(*this)};
    }
    void unhandled_exception() {
    }
    int return_value(int val) {
      _value = val;
      return val;
    };
    auto yield_value(int val) {
      _value = val;
      return std::suspend_always{};
    };
    // void return_void() {
    // }
    int get_return_value() {
      return _value;
    }
    int _value{};
  };

  using promise_type = Promise;
  Task(std::coroutine_handle<promise_type> coroutine_handle)
    : _coroutine_handle(coroutine_handle) {
  }
  ~Task() {
    if (_coroutine_handle) {
      _coroutine_handle.destroy();
    }
  }
  bool await_ready() {
    return false;
  }
  void await_suspend(std::coroutine_handle<promise_type> coroutine_handle) {
    HandleInfo handle_info{
      .id = 0,
      .handle = new CoRoutineHandler(coroutine_handle),
    };
    GetSchedule::get_instance().schedule_now(handle_info);
  }
  // shedule调用完成后 会调用这个函数
  int await_resume() {
    return _coroutine_handle.promise().get_return_value();
  }

  auto get_handle() {
    return _coroutine_handle;
  }
  void resume() {
    if (_coroutine_handle.done()) {
      return;
    }
    _coroutine_handle.resume();
  }
  auto get_return_value() {
    return _coroutine_handle.promise().get_return_value();
  }
  //  private:
  std::coroutine_handle<promise_type> _coroutine_handle;
};
}  // namespace ZhouBoTong
}  // namespace lz