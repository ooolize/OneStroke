/*
 * @Description:
 * @Author: lize
 * @Date: 2024-08-28
 * @LastEditors: lize
 */

#pragma once
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <sched.h>
#include <stdio.h>
#include <sys/syscall.h>
#include <unistd.h>
namespace lz {
namespace system {

inline pid_t gettid() {
  return static_cast<pid_t>(syscall(SYS_gettid));
}

inline void setCPUAffinity(int cpu) {
  cpu_set_t cpuset;
  CPU_ZERO(&cpuset);      // 清空 CPU 集合
  CPU_SET(cpu, &cpuset);  // NOLINT

  // 获取线程 ID
  pthread_t current_thread = pthread_self();

  // 设置线程的 CPU 亲和性
  if (pthread_setaffinity_np(current_thread, sizeof(cpu_set_t), &cpuset) != 0) {
    perror("Error setting CPU affinity");
  }
}

// 设置文件描述符为非阻塞模式
inline int set_nonblocking(int fd) {
  int flags = fcntl(fd, F_GETFL, 0);
  if (flags == -1) {
    perror("fcntl(F_GETFL) failed");
    return -1;
  }

  flags |= O_NONBLOCK;
  if (fcntl(fd, F_SETFL, flags) == -1) {
    perror("fcntl(F_SETFL) failed");
    return -1;
  }
  return 0;
}

}  // namespace system
}  // namespace lz