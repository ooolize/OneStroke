/*
 * @Description:
 * @Author: lize
 * @Date: 2024-10-13
 * @LastEditors: lize
 */

// 一个链接一个协程
// epoll 创建 注册socket 销毁
// 初始化schedule时根据传入地址创建socket并bind listen
// 初始化epoll控制模块 注册刚刚的socket

// loop里 accept 如果新的就schedule_task一个新的协程
// 如果已有链接就绪了 就恢复一个echo协程

// echo协程

#pragma once
#include <fmt/core.h>

#include <chrono>
#include <coroutine>

#include "schedule.h"
#include "task.h"
namespace lz {
namespace OneStroke {

// 处理客户端发送的数据并回显
constexpr int BUFFER_SIZE = 1024;
void echo_func(int client_fd) {
  char buffer[BUFFER_SIZE];
  int bytes_read{};

  while ((bytes_read = read(client_fd, buffer, sizeof(buffer))) > 0) {
    // 将数据回显给客户端
    if (write(client_fd, buffer, bytes_read) == -1) {
      perror("write");
      close(client_fd);
      return;
    }
  }

  if (bytes_read == -1 && errno != EAGAIN) {
    perror("read");
    close(client_fd);
  }

  if (bytes_read == 0) {
    // 客户端关闭连接
    fmt::print("Client closed connection\n");
    close(client_fd);
  }
}

class EchoAwaiter {
 public:
  explicit EchoAwaiter(SocketID client_fd) : _client_fd(client_fd) {
  }
  bool await_ready() {
    return false;
  }
  template <typename Promise>
  void await_suspend(std::coroutine_handle<Promise> coroutine_handle) {
    fmt::print("echo\n");
  }
  void await_resume() {
    echo_func(_client_fd);
  }

 private:
  SocketID _client_fd{};
};

Task<void> echo(SocketID client_fd) {
  co_await EchoAwaiter(client_fd);
}
}  // namespace OneStroke
}  // namespace lz
