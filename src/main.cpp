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

void test3() {
  fmt::print("{}\n", "before test3");
  auto ret = run(test_sleep());
  fmt::print("{}\n", "after test3");
}

Task<void> deep3(WaitForInit) {
  fmt::print("{}\n", "before deep3");
  co_await frame_stack();
  fmt::print("{}\n", "after deep3");
}

Task<void> deep2(WaitForInit) {
  fmt::print("{}\n", "before deep2");
  co_await deep3(wait_for_init);  // task对象的生存周期就一行吗
  fmt::print("{}\n", "after deep2");
}

Task<void> deep1(WaitForInit) {
  fmt::print("{}\n", "before deep1");
  co_await deep2(wait_for_init);
  fmt::print("{}\n", "after deep1");
}
Task<void> test_frame_stack() {
  fmt::print("{}\n", "before test4");
  co_await deep1(wait_for_init);
  fmt::print("{}\n", "after test4");
}

int main() {
  GetSchedule::get_instance().Start();
  std::this_thread::sleep_for(std::chrono::seconds(1));
  // test1();
  // test2();
  // test3();
  auto p = test_frame_stack();
  std::this_thread::sleep_for(std::chrono::seconds(10000));
  return 0;
}
