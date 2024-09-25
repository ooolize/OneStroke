/*
 * @Description:
 * @Author: lize
 * @Date: 2024-09-24
 * @LastEditors: lize
 */

#include <coroutine>

#include "task.h"

namespace lz {
namespace ZhouBoTong {

class SheduleTaskAwaiter {
 public:
  using promise_type = Task::promise_type;
  SheduleTaskAwaiter(Task&& task) : _task(std::move(task)) {
  }
  Task& operator co_await() {
    return _task;
  }

 private:
  Task _task;
};

SheduleTaskAwaiter shedule_task(Task&& task) {
  return SheduleTaskAwaiter{std::move(task)};
}

// Task create_task() {
//   co_return 42;
// }

}  // namespace ZhouBoTong
}  // namespace lz