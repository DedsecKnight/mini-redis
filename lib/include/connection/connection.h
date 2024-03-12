#pragma once

#include <include/protocol/message.h>
#include <sys/socket.h>

#include <optional>

namespace lib::connection {
class connection {
 public:
  enum class connection_state {
    uninitialized,
    initialized,
    reading,
    writing,
    ended
  };

  connection(int sockfd, sockaddr_storage&& addr);
  connection(const connection&) = delete;
  connection& operator=(const connection&) = delete;
  connection(connection&&) = delete;
  connection& operator=(connection&&) = delete;
  ~connection();

  std::optional<protocol::message> get_next_msg() const noexcept;
  int send_msg(const protocol::message& msg) const noexcept;
  int enable_nonblocking_io() const noexcept;

 protected:
  int sockfd_;
  sockaddr_storage sock_addr_;
  connection_state state_{connection_state::uninitialized};

  connection() = default;
  int send(const char* buffer, size_t bytes_sent) const noexcept;
  int receive(char* buffer, size_t expected_msg_size) const noexcept;
};
}  // namespace lib::connection