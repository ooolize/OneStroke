/*
 * @Description:
 * @Author: lize
 * @Date: 2024-09-23
 * @LastEditors: lize
 */
#include <fmt/core.h>

#include <iostream>
#include <thread>

#include "use_func/shedule_task.h"
#include "use_func/sleep.h"
using namespace lz::ZhouBoTong;  // NOLINT

// Task func1() {
//   std::this_thread::sleep_for(std::chrono::seconds(1));

//   auto awaiter = shedule_task(create_task());
//   auto ret = 1;
//   fmt::print("ret = {}\n", ret);
//   std::this_thread::sleep_for(std::chrono::seconds(1));
//   co_await awaiter;
//   fmt::print("ret = {}\n", ret);
// }

Task<void> test_sleep() {
  co_await co_sleep(std::chrono::seconds(2));
  std::cout << "Inside coroutine\n";
}

int main() {
  GetSchedule::get_instance().Start();
  std::this_thread::sleep_for(std::chrono::seconds(1));

  // 这是错误的 为什么
  // test_sleep();

  // 这才是正确的
  auto task1 = test_sleep();

  fmt::print("{}\n", "do some thing");
  task1.resume();

  std::this_thread::sleep_for(std::chrono::seconds(10));
  return 0;
}
