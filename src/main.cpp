/*
 * @Description:
 * @Author: lize
 * @Date: 2024-09-23
 * @LastEditors: lize
 */
#include <fmt/core.h>

#include <thread>

#include "use_func/shedule_task.h"
using namespace lz::ZhouBoTong;

int main() {
  Schedule schedule{};
  std::this_thread::sleep_for(std::chrono::seconds(1));
  auto task = create_task();
  int ret = shedule_task(task);
  fmt::print("ret = {}\n", ret);
  std::this_thread::sleep_for(std::chrono::seconds(1));
  fmt::print("ret = {}\n", ret);

  return 0;
}
