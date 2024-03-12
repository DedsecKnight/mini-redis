#include "include/connection/connection.h"

#include <fcntl.h>
#include <sys/poll.h>
#include <unistd.h>

#include <cassert>
#include <cerrno>
#include <cstdio>
#include <cstring>
#include <utility>

#include "include/connection/poll_manager.h"
#include "include/protocol/message.h"

namespace lib::connection {
connection::connection(int sockfd, sockaddr_storage&& addr)
    : sockfd_{sockfd},
      sock_addr_{std::move(addr)},
      state_{connection_state::initialized} {}

connection::~connection() {
  if (sockfd_ > 0) {
    close(sockfd_);
  }
}

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
    if (curr_recv < 0) {
      perror("recv");
      return -1;
    }
    if (curr_recv == 0) {
      fprintf(stderr, "EOF received\n");
      break;
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
int connection::enable_nonblocking_io() const noexcept {
  errno = 0;
  int flags = fcntl(sockfd_, F_GETFL, 0);
  if (errno) {
    perror("fcntl");
    return -1;
  }
  flags |= O_NONBLOCK;
  errno = 0;
  fcntl(sockfd_, F_SETFL, flags);
  if (errno) {
    perror("fcntl");
    return -1;
  }
  return 0;
}
void connection::register_self_to_poll_manager(
    poll_manager& manager) const noexcept {
  if (state_ == connection_state::ended ||
      state_ == connection_state::uninitialized) {
    return;
  }
  manager.register_connection_cb(
      sockfd_,
      POLLERR | (state_ == connection_state::reading ? POLLIN : POLLOUT));
}
connection::connection_state connection::get_state() const noexcept {
  return state_;
}
void connection::set_state(connection::connection_state new_state) noexcept {
  state_ = new_state;
}
void connection::process_connection() {
  assert(state_ == connection_state::reading ||
         state_ == connection_state::writing);
  if (state_ == connection_state::reading) {
    on_read_state();
  } else {
    on_write_state();
  }
}

bool connection::new_message_available() const noexcept {
  if (read_offset_ < sizeof(protocol::message::msg_size)) {
    return false;
  }
  int len = 0;
  memcpy(&len, read_buffer_, sizeof(protocol::message::msg_size));
  if (len + 4 > read_offset_) {
    return false;
  }
  return true;
}

void connection::on_read_state() noexcept {
  // loop until can no longer read
  while (state_ == connection_state::reading) {
    ssize_t rv = 0;
    do {
      int capacity = sizeof(read_buffer_) - read_offset_;
      rv = ::recv(sockfd_, &read_buffer_[read_offset_], capacity, 0);
    } while (rv < 0 && errno == EINTR);
    if (rv < 0) {
      if (errno == EAGAIN) {
        return;
      } else {
        perror("recv");
        exit(1);
      }
    } else if (rv == 0) {
      fprintf(stderr, "EOF from client\n");
      set_state(connection_state::ended);
      return;
    } else {
      read_offset_ += static_cast<size_t>(rv);
      while (state_ == connection_state::reading && new_message_available()) {
        // TODO: improve process message
        auto msg = peek_message();
        printf("from client [%d bytes]: %s\n", msg.msg_size, msg.msg_content);
        consume_message();
      }
    }
  }
}

void connection::on_write_state() noexcept {
  // loop until everything in write buffer is sent or can no longer write
  while (state_ == connection_state::writing) {
    ssize_t rv = 0;
    do {
      size_t capacity = write_offset_ - write_sent_;
      rv = ::send(sockfd_, &write_buffer_[write_sent_], capacity, 0);
    } while (rv < 0 && errno == EINTR);
    if (rv < 0) {
      if (errno == EAGAIN) {
        return;
      } else {
        perror("send");
        exit(1);
      }
    } else {
      write_sent_ += static_cast<size_t>(rv);
      // if everything is buffer is written, then transition to read state
      if (write_sent_ == write_offset_) {
        memset(write_buffer_, 0, write_offset_);
        write_offset_ = write_sent_ = 0;
        set_state(connection_state::reading);
        return;
      }
    }
  }
}
void connection::destroy() noexcept { close(sockfd_); }
protocol::message connection::peek_message() const noexcept {
  protocol::message msg;
  memcpy(&msg.msg_size, read_buffer_, sizeof(msg.msg_size));
  memcpy(msg.msg_content, &read_buffer_[sizeof(msg.msg_size)], msg.msg_size);
  return msg;
}
void connection::consume_message() noexcept {
  int msg_size;
  memcpy(&msg_size, read_buffer_, sizeof(msg_size));
  memcpy(write_buffer_, &msg_size, sizeof(msg_size));
  memcpy(&write_buffer_[sizeof(msg_size)], &read_buffer_[sizeof(msg_size)],
         msg_size);
  write_offset_ += sizeof(msg_size) + msg_size;
  assert(msg_size + sizeof(msg_size) <= read_offset_);
  auto remain = read_offset_ - msg_size - sizeof(msg_size);
  if (remain > 0) {
    memmove(read_buffer_, &read_buffer_[msg_size + sizeof(msg_size)], remain);
  }
  read_offset_ = remain;
  set_state(connection_state::writing);
  on_write_state();
}
int connection::get_id() const noexcept { return sockfd_; }
connection::connection(connection&& other) {
  sockfd_ = other.sockfd_;
  sock_addr_ = other.sock_addr_;
  state_ = other.state_;
  if (read_offset_) {
    memmove(read_buffer_, other.read_buffer_, other.read_offset_);
  }
  if (write_offset_) {
    memmove(write_buffer_, other.write_buffer_, other.write_offset_);
  }
  read_offset_ = other.read_offset_;
  write_offset_ = other.write_offset_;
  write_sent_ = other.write_sent_;
  other.sockfd_ = -1;
}
connection& connection::operator=(connection&& other) {
  sockfd_ = other.sockfd_;
  sock_addr_ = other.sock_addr_;
  state_ = other.state_;
  if (read_offset_) {
    memmove(read_buffer_, other.read_buffer_, other.read_offset_);
  }
  if (write_offset_) {
    memmove(write_buffer_, other.write_buffer_, other.write_offset_);
  }
  read_offset_ = other.read_offset_;
  write_offset_ = other.write_offset_;
  write_sent_ = other.write_sent_;
  other.sockfd_ = -1;
  return *this;
}
}  // namespace lib::connection