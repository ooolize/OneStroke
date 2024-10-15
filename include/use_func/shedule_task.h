/*
 * @Description:
 * @Author: lize
 * @Date: 2024-09-24
 * @LastEditors: lize
 */

#include <coroutine>

#include "sleep.h"
#include "task.h"
#include "use_concept.h"
namespace lz {
namespace OneStroke {

template <lz::concepts::Future Task>
class SheduleTaskAwaiter {
 public:
  using promise_type = Task::promise_type;
  template <lz::concepts::Future T>
  SheduleTaskAwaiter(T&& task) : _task(std::forward<T>(task)) {
    // add task to schedule
    HandleInfo handle_info(
      task.get_handle_id(),
      std::make_unique<CoRoutineHandler>(task.get_handle()));
    GetSchedule::get_instance().schedule_now(std::move(handle_info));
  }

  HandleID get_handle_id() const {
    return _handle_id;
  }
  decltype(auto) operator co_await() {
    // return co_await _task;  // 返回 _task 的 awaiter
    // return _task.operator co_await();  // 返回 _task 的 awaiter
    return _task;
  }

 private:
  Task _task;
  HandleID _handle_id;
};

// 模板参数推导指引
template <lz::concepts::Future T>
SheduleTaskAwaiter(T&&) -> SheduleTaskAwaiter<T>;

// 为了并发启动多个任务
// template <typename T>
// SheduleTaskAwaiter<T> shedule_task(T&& task) {
//   return SheduleTaskAwaiter{std::forward<T>(task)};
// }

template <typename T>
SheduleTaskAwaiter<T> shedule_task(T&& task) {
  return SheduleTaskAwaiter<T>{std::forward<T>(task)};
}

Task<void> test_sleep() {
  std::cout << "before sleep\n";
  co_await co_sleep(std::chrono::seconds(2));
  std::cout << "Inside coroutine\n";
}

}  // namespace OneStroke
}  // namespace lz