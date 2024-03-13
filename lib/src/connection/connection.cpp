#include "include/connection/connection.h"

#include <fcntl.h>
#include <sys/poll.h>
#include <unistd.h>

#include <cassert>
#include <cerrno>
#include <cstdio>
#include <cstring>
#include <optional>
#include <utility>

#include "include/connection/poll_manager.h"
#include "include/protocol/message.h"
#include "include/protocol/request.h"
#include "include/protocol/response.h"

namespace lib::connection {
connection::connection(int sockfd, sockaddr_storage&& addr)
    : sockfd_{sockfd},
      sock_addr_{std::move(addr)},
      state_{connection_state::initialized} {
  memset(read_buffer_, 0, sizeof(read_buffer_));
  memset(write_buffer_, 0, sizeof(write_buffer_));
}

connection::~connection() {
  if (sockfd_ > 0) {
    close(sockfd_);
  }
}

int connection::send(char* buffer, size_t bytes_sent) const noexcept {
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
      while (state_ == connection_state::reading && new_request_available()) {
        assert(owner_server_ != nullptr);
        owner_server_->on_request_available_cb(peek_request(), *this);
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
void connection::consume_buffer(size_t buff_size) noexcept {
  assert(buff_size <= read_offset_);
  auto remain = read_offset_ - buff_size;
  if (remain > 0) {
    memmove(read_buffer_, &read_buffer_[buff_size], remain);
  }
  read_offset_ = remain;
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
  owner_server_ = other.owner_server_;
  other.owner_server_ = nullptr;
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
  owner_server_ = other.owner_server_;
  other.sockfd_ = -1;
  other.owner_server_ = nullptr;
  return *this;
}
bool connection::new_request_available() const noexcept {
  if (read_offset_ < sizeof(int)) {
    return false;
  }
  int num_msg = 0;
  memcpy(&num_msg, read_buffer_, sizeof(int));
  assert(num_msg > 0);
  for (int i = 0, ptr = sizeof(int); i < num_msg; i++) {
    if (ptr + sizeof(int) > read_offset_) {
      return false;
    }
    int curr_msg_size;
    memcpy(&curr_msg_size, &read_buffer_[ptr], sizeof(int));
    if (ptr + sizeof(int) + curr_msg_size > read_offset_) {
      return false;
    }
    ptr += sizeof(int) + curr_msg_size;
  }
  return true;
}
protocol::request connection::peek_request() const noexcept {
  protocol::request req;
  int num_msg;
  memcpy(&num_msg, read_buffer_, sizeof(int));
  for (int i = 0, ptr = sizeof(int); i < num_msg; i++) {
    protocol::message curr_msg;
    memset(&curr_msg, 0, sizeof(curr_msg));
    memcpy(&curr_msg.msg_size, &read_buffer_[ptr], sizeof(int));
    memcpy(curr_msg.msg_content, &read_buffer_[ptr + sizeof(int)],
           curr_msg.msg_size);
    ptr += sizeof(int) + curr_msg.msg_size;
    req.add_message(std::move(curr_msg));
  }
  assert(req.num_messages() == num_msg);
  return req;
}
int connection::send_request(const protocol::request& req) const noexcept {
  if (state_ == connection_state::uninitialized) {
    fprintf(stderr, "connection uninitialized\n");
    return -1;
  }
  if (req.size() > 4096) {
    fprintf(stderr, "request too large\n");
    return -1;
  }
  auto serialized_req = req.serialize();
  return connection::send(serialized_req.get(), req.size());
}
void connection::nonblocking_send(char* buffer, size_t bytes_sent) noexcept {
  if (bytes_sent > sizeof(write_buffer_)) {
    fprintf(stderr, "data too large\n");
    return;
  }
  assert(write_offset_ == 0);
  memcpy(write_buffer_, buffer, bytes_sent);
  write_offset_ += bytes_sent;
  set_state(connection_state::writing);
  on_write_state();
}
std::optional<protocol::request> connection::get_next_request() const noexcept {
  int num_msg;
  if (connection::receive(reinterpret_cast<char*>(&num_msg), sizeof(int)) ==
      -1) {
    return std::nullopt;
  }
  protocol::request req;
  for (int i = 0; i < num_msg; i++) {
    if (auto msg_opt = connection::get_next_msg(); msg_opt != std::nullopt) {
      req.add_message(msg_opt.value());
    } else {
      return std::nullopt;
    }
  }
  return req;
}
void connection::register_self_to_server(
    interface::server* owner_server) noexcept {
  owner_server_ = owner_server;
}
std::optional<protocol::response> connection::get_next_response()
    const noexcept {
  char msg[4096];
  int msg_size;
  protocol::response_code code;
  memset(msg, 0, sizeof(msg));
  if (connection::receive(reinterpret_cast<char*>(&code), sizeof(code)) == -1) {
    return std::nullopt;
  }
  if (connection::receive(reinterpret_cast<char*>(&msg_size),
                          sizeof(msg_size)) == -1) {
    return std::nullopt;
  }
  if (connection::receive(msg, msg_size - sizeof(protocol::response_code) -
                                   sizeof(msg_size)) == -1) {
    return std::nullopt;
  }
  std::string ret;
  ret.append(msg);
  return protocol::response{code, ret};
}
}  // namespace lib::connection