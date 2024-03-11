#include "include/connection/connection.h"

#include <unistd.h>

#include <cstdio>
#include <utility>

namespace lib::connection {
connection::connection(int sockfd, sockaddr_storage&& addr)
    : sockfd_{sockfd}, sock_addr_{std::move(addr)} {}

connection::~connection() { close(sockfd_); }

int connection::send(const char* buffer, size_t bytes_sent) const noexcept {
  size_t sent = 0;
  while (sent < bytes_sent) {
    ssize_t curr_sent = ::send(sockfd_, &buffer[sent], bytes_sent - sent, 0);
    if (curr_sent < 0) {
      perror("send");
      return -1;
    }
    sent += static_cast<size_t>(curr_sent);
  }
  return 0;
}

int connection::receive(char* buffer, size_t expected_msg_size) const noexcept {
  size_t received = 0;
  while (received < expected_msg_size) {
    ssize_t curr_recv =
        ::recv(sockfd_, &buffer[received], expected_msg_size - received, 0);
    if (curr_recv <= 0) {
      perror("recv");
      return -1;
    }
    received += static_cast<size_t>(curr_recv);
  }
  return 0;
}

}  // namespace lib::connection