/*
 * @Description:
 * @Author: lize
 * @Date: 2024-09-24
 * @LastEditors: lize
 */
#pragma once

#include <coroutine>
#include <utility>

#include "interface/handle.h"
#include "schedule.h"
#include "tag.h"
namespace lz {
namespace ZhouBoTong {

template <typename R>
class Result {
 public:
  void return_value(R value) {
    _result = value;
  }
  void unhandled_exception() {
    _result = std::current_exception();
  }
  R get_return_value() {
    if (std::holds_alternative<std::exception_ptr>(_result)) {
      std::rethrow_exception(std::get<std::exception_ptr>(_result));
    }
    return std::get<R>(_result);
  }

 private:
  std::variant<R, std::exception_ptr> _result{};
};

template <>
class Result<void> {
 public:
  void return_void() {
  }
  void unhandled_exception() {
    _result = std::current_exception();
  }

  decltype(auto) get_return_value() {
    if (_result) {
      std::rethrow_exception(*_result);
    }
    return std::nullopt;
  }

 private:
  std::optional<std::exception_ptr> _result{};
};

template <typename R>
class Task {
 public:
  class Promise : public Result<R> {
   public:
    Promise() = default;

    // 自由函数
    template <typename... Args>
    Promise(NoWaitForInit no_wait_for_init, Args... args)
      : is_suspend_init(false) {
    }

    // 成员函数 lambda
    template <typename Obj, typename... Args>
    Promise(Obj&& obj, NoWaitForInit no_wait_for_init, Args... args)
      : is_suspend_init(false) {
    }
    auto get_return_object() {
      return Task<R>{std::coroutine_handle<Promise>::from_promise(*this)};
    }
    // Promise() = default;
    // 协程首次挂起前不会挂起，继续执行
    auto initial_suspend() {
      class InitAwaiter {
       public:
        bool await_ready() {
          return !is_suspend_init;
        }
        void await_suspend(std::coroutine_handle<Promise> coroutine_handle) {
        }
        void await_resume() {
        }
        bool is_suspend_init{true};
      };
      return InitAwaiter{is_suspend_init};
    }

    // 协程完成时挂起 final_suspend，保持协程状态
    std::suspend_always final_suspend() noexcept {
      return {};
    }

    // auto yield_value(int val) {
    //   _value = val;
    //   return std::suspend_always{};
    // };
    bool is_suspend_init{true};
    // int _value{};
  };

  using promise_type = Promise;
  explicit Task(std::coroutine_handle<promise_type> coroutine_handle)
    : _coroutine_handle(coroutine_handle) {
  }
  ~Task() {
    if (_coroutine_handle && _coroutine_handle.done()) {
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
  void await_resume() {
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
  std::coroutine_handle<promise_type> _coroutine_handle{};
};
}  // namespace ZhouBoTong
}  // namespace lz