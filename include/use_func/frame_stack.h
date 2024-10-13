/*
 * @Description:
 * @Author: lize
 * @Date: 2024-10-09
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

class FrameStackAwaiter {
 public:
  bool await_ready() {
    return false;
  }

  // 这里是bool 不挂起当前协程 直接到resume
  template <typename Promise>
  bool await_suspend(std::coroutine_handle<Promise> handle) const noexcept {
    handle.promise().frame_stack();
    return false;
  }
  void await_resume() {
    fmt::print("frame_stack\n");
  }
};

// 打印协程调用栈
// [[nodiscord("should be used in co_await")]]
FrameStackAwaiter frame_stack() {
  return FrameStackAwaiter{};
}

Task<void> deep3(WaitForInit) {
  fmt::print("{}\n", "before deep3");
  co_await frame_stack();
  fmt::print("{}\n", "after deep3");
}

Task<void> deep2(WaitForInit) {
  fmt::print("{}\n", "before deep2");
  co_await deep3(wait_for_init);  // task对象的生存周期就一行吗
  fmt::print("{}\n", "after deep2");
}

Task<void> deep1(WaitForInit) {
  fmt::print("{}\n", "before deep1");
  co_await deep2(wait_for_init);
  fmt::print("{}\n", "after deep1");
}

}  // namespace ZhouBoTong
}  // namespace lz
