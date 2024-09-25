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
using namespace lz::ZhouBoTong;

// Task func1() {
//   std::this_thread::sleep_for(std::chrono::seconds(1));

//   auto awaiter = shedule_task(create_task());
//   auto ret = 1;
//   fmt::print("ret = {}\n", ret);
//   std::this_thread::sleep_for(std::chrono::seconds(1));
//   co_await awaiter;
//   fmt::print("ret = {}\n", ret);
// }

Task test_sleep() {
  // co_await co_sleep(std::chrono::seconds(2));
  co_yield 42;
  std::cout << "Inside coroutine\n";
  co_yield 43;
  co_yield 44;
}

int main() {
  // GetSchedule::get_instance().Start();
  // std::this_thread::sleep_for(std::chrono::seconds(1));
  // func1();
  Task p = test_sleep();
  auto promise = p._coroutine_handle.promise();
  std::cout << "before resume\n";
  // std::this_thread::sleep_for(std::chrono::seconds(1));
  std::cout << "ret = " << promise.get_return_value() << std::endl;

  p.resume();
  std::cout << "ret = " << promise.get_return_value() << std::endl;

  p.resume();
  std::cout << "ret = " << promise.get_return_value() << std::endl;
  // p._coroutine_handle.destroy();
  // std::this_thread::sleep_for(std::chrono::seconds(10));
  return 0;
}
