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
Task detail_shedule_task(Task task) {
  co_await task;
}

int shedule_task(Task task) {
  Task new_task = detail_shedule_task(task);
  return new_task.get_return_value();
}

Task create_task() {
  co_return 42;
}

}  // namespace ZhouBoTong
}  // namespace lz