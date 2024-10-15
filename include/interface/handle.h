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
#include <memory>

#include "utils/empty_base.h"
namespace lz {
namespace ZhouBoTong {

class Handle;
using HandleID = std::size_t;
using SocketID = std::int32_t;
using TimePoint = std::chrono::time_point<std::chrono::steady_clock>;
using Duration = std::chrono::duration<int64_t, std::nano>;
using HandleUPtr = std::unique_ptr<Handle>;
using HandleSPtr = std::shared_ptr<Handle>;

inline HandleID getNextId() {
  static HandleID id = 0;
  return id++;
}

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
  virtual HandleID get_handle_id() const = 0;
};

class CoRoutineHandler : public Handle {
 public:
  CoRoutineHandler() : _handle_id(getNextId()) {
  }
  CoRoutineHandler(std::coroutine_handle<> coroutine_handle)
    : _coroutine_handle(coroutine_handle), _handle_id(getNextId()) {
  }
  void run() final {
    if (_coroutine_handle.done()) {
      fmt::print("coroutine done\n");
      return;
    }
    _coroutine_handle.resume();
  }
  virtual HandleID get_handle_id() const {
    return _handle_id;
  }

 private:
  std::coroutine_handle<> _coroutine_handle{};
  HandleID _handle_id{};
};

struct HandleInfo {
  HandleInfo() = default;
  HandleInfo(HandleID id,
             HandleUPtr handle,
             HandleState state = HandleState::kReady)
    : id(id), handle(std::move(handle)), state(state) {
  }
  HandleInfo(HandleInfo&) = delete;
  HandleInfo& operator=(HandleInfo&) = delete;

  HandleInfo(HandleInfo&&) = default;
  HandleInfo& operator=(HandleInfo&&) = default;

  HandleID id{};
  HandleUPtr handle = nullptr;
  HandleState state = HandleState::kReady;
};

}  // namespace ZhouBoTong
}  // namespace lz