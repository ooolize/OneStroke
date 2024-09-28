/*
 * @Description:
 * @Author: lize
 * @Date: 2024-09-24
 * @LastEditors: lize
 */
#pragma once
#include <fmt/core.h>

#include <chrono>
#include <coroutine>
#include <cstdint>

#include "utils/empty_base.h"
namespace lz {
namespace ZhouBoTong {

using HandleID = std::size_t;
using TimePoint = std::chrono::time_point<std::chrono::steady_clock>;
using Duration = std::chrono::duration<int64_t, std::nano>;

enum class HandleState : uint8_t {
  kReady,
  kWait,
  kCancel,
};
// 问题 对于接口类 我们需要一个public的虚析构函数
// 。但我们需要考虑它的所有权关系吗？
// 我认为是不需要的，因为它不会实例化，但core
// guidline中定义析构函数必须明确指出其他4个的行为
class Handle {
 public:
  virtual ~Handle() = default;
  virtual void run() = 0;
};

class CoRoutineHandler : public Handle {
 public:
  CoRoutineHandler(std::coroutine_handle<> coroutine_handle)
    : _coroutine_handle(coroutine_handle) {
  }
  void run() final {
    if (_coroutine_handle.done()) {
      fmt::print("coroutine done\n");
      return;
    }
    _coroutine_handle.resume();
  }

 private:
  std::coroutine_handle<> _coroutine_handle{};
};
struct HandleInfo {
  HandleID id{};
  Handle* handle = nullptr;
  HandleState state = HandleState::kReady;
};

inline HandleID getNextId() {
  static HandleID id = 0;
  return id++;
}

}  // namespace ZhouBoTong
}  // namespace lz