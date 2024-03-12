#pragma once

#include <include/protocol/message.h>
#include <sys/socket.h>

#include <optional>

#include "include/connection/poll_manager.h"

namespace lib::connection {
class connection {
 public:
  static constexpr const int MAX_BUFFER_SIZE = 4096;
  enum class connection_state {
    uninitialized,
    initialized,
    reading,
    writing,
    ended
  };

  connection() = default;
  connection(int sockfd, sockaddr_storage&& addr);
  connection(const connection&) = delete;
  connection& operator=(const connection&) = delete;
  connection(connection&&);
  connection& operator=(connection&&);
  ~connection();

  std::optional<protocol::message> get_next_msg() const noexcept;
  int send_msg(const protocol::message& msg) const noexcept;
  int enable_nonblocking_io() const noexcept;
  void register_self_to_poll_manager(poll_manager& manager) const noexcept;
  connection_state get_state() const noexcept;
  void set_state(connection_state new_state) noexcept;
  void process_connection();
  void destroy() noexcept;
  protocol::message peek_message() const noexcept;
  void consume_message() noexcept;
  int get_id() const noexcept;

 private:
  void on_read_state() noexcept;
  void on_write_state() noexcept;
  bool new_message_available() const noexcept;

  char read_buffer_[MAX_BUFFER_SIZE];
  char write_buffer_[MAX_BUFFER_SIZE];
  size_t read_offset_{0}, write_offset_{0}, write_sent_{0};

 protected:
  int sockfd_;
  sockaddr_storage sock_addr_;
  connection_state state_{connection_state::uninitialized};

  int send(const char* buffer, size_t bytes_sent) const noexcept;
  int receive(char* buffer, size_t expected_msg_size) const noexcept;
};
}  // namespace lib::connection