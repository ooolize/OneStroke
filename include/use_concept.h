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
concept Awaitable = requires(T t) { t.operator co_await(); };

template <typename T>
concept Awaiter = Awaitable<T> || requires(T t) {
  { t.await_ready() } -> std::convertible_to<bool>;
  // TODO t.await_suspend(coroutine_handle);
  t.await_resume();
};

template <typename T>
concept Future = requires(T t) {
  typename T::promise_type;
  // { t._coroutine_handle.promise() } -> std::same_as<typename
  // T::promise_type>;
};

template <typename T>
concept Promise = requires(T t) {
  t.initial_suspend();
  t.final_suspend();
  t.get_return_object();
};

}  // namespace concepts
}  // namespace lz