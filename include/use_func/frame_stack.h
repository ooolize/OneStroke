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

FrameStackAwaiter frame_stack() {
  return FrameStackAwaiter{};
}

}  // namespace ZhouBoTong
}  // namespace lz
