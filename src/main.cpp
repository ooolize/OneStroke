/*
 * @Description:
 * @Author: lize
 * @Date: 2024-09-23
 * @LastEditors: lize
 */
#include <fmt/core.h>

#include <iostream>
#include <thread>

#include "use_func/frame_stack.h"
#include "use_func/run.h"
#include "use_func/shedule_task.h"
#include "use_func/sleep.h"
#include "use_func/wait_for.h"
using namespace lz::OneStroke;  // NOLINT

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

void test3() {
  fmt::print("{}\n", "before test3");
  auto ret = run(test_sleep());
  fmt::print("{}\n", "after test3");
}

Task<void> test_frame_stack() {
  fmt::print("{}\n", "before test4");
  co_await deep1(wait_for_init);
  fmt::print("{}\n", "after test4");
}

Task<void> test_wait_for() {
  fmt::print("{}\n", "before test_wait_for");
  co_await co_wait_for(wait_func(wait_for_init), std::chrono::seconds(1));
  fmt::print("{}\n", "after test_wait_for");
}

int main() {
  GetSchedule::get_instance().Start();
  std::this_thread::sleep_for(std::chrono::seconds(1));
  // test1();
  // test2();
  // test3();
  // auto p = test_frame_stack();
  // auto p = test_wait_for();
  std::this_thread::sleep_for(std::chrono::seconds(10000));
  return 0;
}