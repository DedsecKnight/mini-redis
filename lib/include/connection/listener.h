#pragma once

#include <sys/socket.h>

#include <optional>
#include <string_view>

#include "include/connection/connection.h"
#include "include/connection/poll_manager.h"

namespace lib::connection {
class listener : public connection {
 private:
  static constexpr const int MAX_CONNECTION_ALLOWED = 128;
  listener(int sockfd, sockaddr_storage&& addr);

 public:
  listener(std::string_view hostname, std::string_view port);
  int listen() const noexcept;
  void register_listener_to_poll_manager(poll_manager& manager) noexcept;
  std::optional<connection> accept_new_listener() const noexcept;

  listener(const listener&) = delete;
  listener& operator=(const listener&) = delete;

  listener(listener&&);
  listener& operator=(listener&&);
};
}  // namespace lib::connection