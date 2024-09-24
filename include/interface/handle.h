/*
 * @Description:
 * @Author: lize
 * @Date: 2024-09-24
 * @LastEditors: lize
 */
#pragma once
#include <cstdint>

#include "utils/empty_base.h"
namespace lz {
namespace ZhouBoTong {

using HandleID = std::size_t;
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

struct HandleInfo {
  HandleID id;
  Handle* handle;
  HandleState state;
};
}  // namespace ZhouBoTong
}  // namespace lz