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
namespace OneStroke {

// 不想把他作为协程 而是在函数中调用协程
template <typename Task>
decltype(auto) run(Task&& task) {
  auto awaiter = shedule_task(std::forward<Task>(task));
  //   co_await awaiter;
  while (!GetSchedule::get_instance().is_empty()) {
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
  return task.get_return_value();
}

}  // namespace OneStroke

}  // namespace lz
