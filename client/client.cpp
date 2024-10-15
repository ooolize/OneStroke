/*
 * @Description:
 * @Author: lize
 * @Date: 2024-10-15
 * @LastEditors: lize
 */
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define SERVER_IP "127.0.0.1"  // 服务器 IP 地址
#define SERVER_PORT 8082       // 服务器端口
#define BUFFER_SIZE 1024       // 缓冲区大小

int main() {
  int sockfd;
  struct sockaddr_in server_addr;
  char send_buffer[BUFFER_SIZE], recv_buffer[BUFFER_SIZE];
  int bytes_received;

  // 创建客户端套接字
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd == -1) {
    perror("socket");
    exit(EXIT_FAILURE);
  }

  // 配置服务器地址
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(SERVER_PORT);

  // 将服务器 IP 转换为二进制
  if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0) {
    perror("inet_pton");
    close(sockfd);
    exit(EXIT_FAILURE);
  }

  // 连接服务器
  if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) ==
      -1) {
    perror("connect");
    close(sockfd);
    exit(EXIT_FAILURE);
  }

  printf("Connected to server at %s:%d\n", SERVER_IP, SERVER_PORT);

  while (1) {
    // 从标准输入读取用户输入
    printf("Enter message to send (or type 'exit' to quit): ");
    fgets(send_buffer, BUFFER_SIZE, stdin);

    // 如果输入 "exit" 则退出
    if (strncmp(send_buffer, "exit", 4) == 0) {
      break;
    }

    // 向服务器发送数据
    if (send(sockfd, send_buffer, strlen(send_buffer), 0) == -1) {
      perror("send");
      close(sockfd);
      exit(EXIT_FAILURE);
    }

    // 从服务器接收回显数据
    bytes_received = recv(sockfd, recv_buffer, BUFFER_SIZE, 0);
    if (bytes_received == -1) {
      perror("recv");
      close(sockfd);
      exit(EXIT_FAILURE);
    } else if (bytes_received == 0) {
      // 服务器关闭连接
      printf("Server closed connection\n");
      close(sockfd);
      break;
    }

    // 确保回显的数据以 '\0' 结尾
    recv_buffer[bytes_received] = '\0';
    printf("Received from server: %s", recv_buffer);
  }

  // 关闭套接字
  close(sockfd);
  printf("Connection closed.\n");
  return 0;
}
