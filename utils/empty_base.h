/*
 * @Description:
 * @Author: lize
 * @Date: 2024-09-24
 * @LastEditors: lize
 */
#pragma once
namespace lz {
namespace empty_base {
class NoCopyable {
 public:
  NoCopyable() = default;
  ~NoCopyable() = default;

 private:
  NoCopyable(const NoCopyable&) = delete;
  NoCopyable& operator=(const NoCopyable&) = delete;

  NoCopyable(NoCopyable&&) = delete;
  NoCopyable& operator=(NoCopyable&&) = delete;
};

class NoMoveable {
 public:
  NoMoveable() = default;
  ~NoMoveable() = default;
  NoMoveable(const NoMoveable&) = default;
  NoMoveable& operator=(const NoMoveable&) = default;

 private:
  NoMoveable(NoMoveable&&) = delete;
  NoMoveable& operator=(NoMoveable&&) = delete;
};
}  // namespace empty_base
}  // namespace lz
