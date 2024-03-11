#include "include/connection/connection.h"

#include <unistd.h>

#include <cstdio>
#include <cstring>
#include <utility>

namespace lib::connection {
connection::connection(int sockfd, sockaddr_storage&& addr)
    : sockfd_{sockfd},
      sock_addr_{std::move(addr)},
      state_{connection_state::initialized} {}

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

std::optional<protocol::message> connection::get_next_msg() const noexcept {
  if (state_ == connection_state::uninitialized) {
    return std::nullopt;
  }
  protocol::message msg;
  if (connection::receive(reinterpret_cast<char*>(&msg.msg_size),
                          sizeof(msg.msg_size)) == -1) {
    return std::nullopt;
  }
  if (connection::receive(msg.msg_content, msg.msg_size) == -1) {
    return std::nullopt;
  }
  return msg;
}

int connection::send_msg(const protocol::message& msg) const noexcept {
  if (state_ == connection_state::uninitialized) {
    fprintf(stderr, "connection uninitialized\n");
    return -1;
  }
  if (msg.msg_size > protocol::message::MAX_MSG_SIZE) {
    fprintf(stderr, "message too large\n");
    return -1;
  }
  char raw_msg_buf[sizeof(msg.msg_size) + msg.msg_size];
  memcpy(raw_msg_buf, &msg.msg_size, sizeof(msg.msg_size));
  memcpy(&raw_msg_buf[sizeof(msg.msg_size)], msg.msg_content,
         static_cast<size_t>(msg.msg_size));
  return connection::send(raw_msg_buf, sizeof(raw_msg_buf));
}

}  // namespace lib::connection