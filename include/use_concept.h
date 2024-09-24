/*
 * @Description:
 * @Author: lize
 * @Date: 2024-09-24
 * @LastEditors: lize
 */

#pragma once
#include <concepts>
#include <coroutine>
namespace lz {
namespace concepts {

template <typename T>
concept Awaiter = requires(T t) {
  { t.await_ready() } -> std::convertible_to<bool>;
  t.await_suspend();
  t.await_resume();
};

template <typename T>
concept Future = requires(T t) {
  typename T::promise_type;
  { t._h.promise() } -> std::same_as<typename T::promise_type>;
};

template <typename T>
concept Promise = requires(T t) {
  t.initial_suspend();
  t.final_suspend();
  t.get_return_object();
};

}  // namespace concepts
}  // namespace lz