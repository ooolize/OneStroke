/*
 * @Description:
 * @Author: lize
 * @Date: 2024-10-15
 * @LastEditors: lize
 */
#pragma once
#include <sys/epoll.h>
// #include "utils/empty_base.h"
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>

#include <unordered_set>

#include "interface/handle.h"
#include "utils/system.h"

namespace lz {
namespace ZhouBoTong {
static constexpr int MAX_EVENT = 1024;
class EpollEvent {
 public:
  EpollEvent() = default;

  ~EpollEvent() {
    close(_epoll_fd);
  }
  EpollEvent(const EpollEvent&) = delete;
  EpollEvent& operator=(const EpollEvent&) = delete;
  EpollEvent(EpollEvent&&) = delete;
  EpollEvent& operator=(EpollEvent&&) = delete;

  void init(int port) {
    // TODO 抽象Socket
    struct sockaddr_in server_addr {};
    // 绑定地址和端口
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    // 创建 TCP 套接字
    _server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (_server_fd == -1) {
      perror("socket");
      exit(EXIT_FAILURE);
    }

    if (bind(_server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) ==
        -1) {
      perror("bind");
      close(_server_fd);
    }

    // 开始监听连接
    if (listen(_server_fd, SOMAXCONN) == -1) {
      perror("listen");
      close(_server_fd);
    }
    lz::system::set_nonblocking(_server_fd);

    if (create_epoll() < 0) {
      return;
    };
    if (!add_event(_server_fd, EPOLLIN)) {
      return;
    };
  }

  SocketID create_epoll() {
    _epoll_fd = epoll_create1(0);
    if (_epoll_fd == -1) {
      perror("epoll_create1");
      return -1;
    }
    return _epoll_fd;
  }
  bool add_event(int fd, int events) {
    struct epoll_event ev {};
    ev.events = events;
    ev.data.fd = fd;
    if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, fd, &ev) == -1) {
      perror("epoll_ctl");
      return false;
    }
    return true;
  }
  void wait_event(int timeout) {
    auto nfds = epoll_wait(_epoll_fd, _events, MAX_EVENT, timeout);
    if (nfds == -1) {
      perror("epoll_wait");
      return;
    }
    // TODO 抽象Socket
    sockaddr_in client_addr{};
    struct epoll_event event {};
    int client_fd = -1;
    socklen_t addrlen = sizeof(client_addr);

    for (int i = 0; i < nfds; i++) {
      if (_events[i].events & EPOLLERR || _events[i].events & EPOLLHUP ||
          !(_events[i].events & EPOLLIN)) {
        // 发生错误或者连接已关闭，关闭文件描述符
        perror("epoll error");
        close(_events[i].data.fd);
        continue;
      } else if (_events[i].data.fd == _server_fd) {
        // 处理新的客户端连接
        while ((client_fd = accept(
                  _server_fd, (struct sockaddr*)&client_addr, &addrlen)) !=
               -1) {
          fmt::print("New connection from {}:{}\n",
                     inet_ntoa(client_addr.sin_addr),
                     ntohs(client_addr.sin_port));

          // 设置客户端套接字为非阻塞
          lz::system::set_nonblocking(client_fd);

          // 将新的客户端套接字添加到 epoll 实例中
          event.data.fd = client_fd;
          event.events = EPOLLIN | EPOLLET;  // 边缘触发模式
          if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, client_fd, &event) == -1) {
            perror("epoll_ctl: client_sock");
            close(client_fd);
          } else {
            _new_fd_set.insert(client_fd);
          }
        }

        if (client_fd == -1 && errno != EAGAIN && errno != EWOULDBLOCK) {
          perror("accept");
        }
      } else {
        // 处理来自客户端的数据
        _ready_fd.insert(_events[i].data.fd);
      }
    }
  }

  void clear() {
    _ready_fd.clear();
    _new_fd_set.clear();
  }

  const auto& get_ready_fd() const {
    return _ready_fd;
  }
  const auto& get_new_fd() const {
    return _new_fd_set;
  }

 private:
  SocketID _epoll_fd{};
  // 关心的socket_id
  SocketID _server_fd{};
  // 就绪的事件
  struct epoll_event _events[MAX_EVENT];
  std::unordered_set<SocketID> _ready_fd{};
  std::unordered_set<SocketID> _new_fd_set{};
};
}  // namespace ZhouBoTong
}  // namespace lz