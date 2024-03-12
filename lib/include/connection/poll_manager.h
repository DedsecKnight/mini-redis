#pragma once

#include <poll.h>

#include <algorithm>
#include <vector>

// #include "include/connection/connection.h"

namespace lib::connection {
class poll_manager {
 private:
  using socket_fd_t = int;
  using polltype_t = int;

 public:
  void register_connection_cb(socket_fd_t fd, polltype_t poll_type);
  void register_listener_cb(socket_fd_t fd);
  int poll(int timeout);
  void reset_args();
  bool new_connection_available() const noexcept;

  void process_active_connection(auto&& f) const noexcept {
    std::for_each(poll_fds_.begin() + 1, poll_fds_.end(),
                  [&f](const auto& pfd) {
                    if (pfd.revents) {
                      f(pfd.fd);
                    }
                  });
  }

 private:
  std::vector<pollfd> poll_fds_;
  size_t listener_fd_index_;
};
}  // namespace lib::connection