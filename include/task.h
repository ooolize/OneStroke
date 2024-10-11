/*
 * @Description:
 * @Author: lize
 * @Date: 2024-09-24
 * @LastEditors: lize
 */
#pragma once

#include <coroutine>
#include <source_location>
#include <utility>

#include "interface/handle.h"
#include "schedule.h"
#include "tag.h"
#include "use_concept.h"
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
    Promise(WaitForInit wait_for_init, Args... args) : is_suspend_init(true) {
    }

    // 成员函数 lambda
    template <typename Obj, typename... Args>
    Promise(Obj&& obj, WaitForInit wait_for_init, Args... args)
      : is_suspend_init(true) {
    }
    auto get_return_object() {
      return Task<R>{std::coroutine_handle<Promise>::from_promise(*this)};
    }
    // Promise() = default;
    // 协程首次挂起前不会挂起，继续执行
    decltype(auto) initial_suspend() {
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

    // 协程完成时挂起 final_suspend，保持协程状态 由task的析构函数销毁协程
    decltype(auto) final_suspend() noexcept {
      class FinalAwaiter {
       public:
        bool await_ready() noexcept {
          return false;
        }
        // 此时coroutine_handle是子协程
        void await_suspend(
          std::coroutine_handle<Promise> coroutine_handle) noexcept {
          auto parent = coroutine_handle.promise()._parent;
          if (!parent) {
            return;
          }
          // 恢复父协程 协程的销毁交给co_await taskA()所触发的task析构函数
          HandleInfo handle_info{
            .id = getNextId(),
            .handle = new CoRoutineHandler(parent),
          };
          GetSchedule::get_instance().schedule_now(handle_info);
        }
        void await_resume() noexcept {
        }
      };
      return FinalAwaiter{};
    }

    // 打印调用栈
    void frame_stack() {
      fmt::print("{}:{}-{}\n",
                 _source_location.file_name(),
                 _source_location.line(),
                 _source_location.function_name());
      if (_parent) {
        _parent.promise().frame_stack();
      } else {
        fmt::print("\n");
      }
    }

    // 每次记录co_await的位置 是这里而不是构造函数 构造函数是协程创建时
    template <lz::concepts::Awaiter T>
    decltype(auto) await_transform(
      T&& awaiter,
      std::source_location source_location = std::source_location::current()) {
      _source_location = source_location;
      return std::forward<T>(awaiter);
    }

    // auto yield_value(int val) {
    //   _value = val;
    //   return std::suspend_always{};
    // };
    bool is_suspend_init{false};
    std::source_location _source_location{};
    std::coroutine_handle<Promise> _parent{};
    // int _value{};
  };
  using result_type = R;
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
      .id = getNextId(),
      .handle = new CoRoutineHandler(_coroutine_handle),
    };
    // 保存当前协程挂起时 保存父协程
    _coroutine_handle.promise()._parent = coroutine_handle;

    // 如果没就绪 何时恢复子协程--->进入队列
    GetSchedule::get_instance().schedule_now(handle_info);
  }
  // shedule遍历队列 调用handle的resume 协程恢复 然后到这里
  void await_resume() {
    std::cout << "await_resume\n";
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
  decltype(auto) get_handle() {
    return _coroutine_handle;
  }
  //  private:
  std::coroutine_handle<promise_type> _coroutine_handle{};
};
}  // namespace ZhouBoTong
}  // namespace lz