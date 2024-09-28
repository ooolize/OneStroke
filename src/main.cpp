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

Task<void> test_shedule_task() {
  auto awaiter = shedule_task(test_sleep());
  std::cout << "do some thing\n";
  co_await awaiter;
}

void test1() {
  std::cout << "before test1" << std::endl;
  auto p = test_shedule_task();
  std::cout << "after test1" << std::endl;
}

void test2() {
  // 这是错误的 为什么
  // test_sleep();

  // 这才是正确的
  auto task1 = test_sleep();
  fmt::print("{}\n", "do some thing");
}

int main() {
  GetSchedule::get_instance().Start();
  std::this_thread::sleep_for(std::chrono::seconds(1));
  test1();
  // test2();
  std::this_thread::sleep_for(std::chrono::seconds(10));
  return 0;
}
