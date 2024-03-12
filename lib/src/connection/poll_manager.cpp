#include "include/connection/poll_manager.h"

#include <sys/poll.h>

#include <cstdio>

namespace lib::connection {
void poll_manager::register_connection_cb(poll_manager::socket_fd_t fd,
                                          poll_manager::polltype_t poll_type) {
  poll_fds_.push_back({fd, static_cast<short>(poll_type), 0});
}
int poll_manager::poll(int timeout) {
  int rv =
      ::poll(poll_fds_.data(), static_cast<nfds_t>(poll_fds_.size()), 1000);
  if (rv < 0) {
    perror("poll");
  }
  return rv;
}
void poll_manager::reset_args() { poll_fds_.clear(); }
void poll_manager::register_listener_cb(poll_manager::socket_fd_t fd) {
  poll_fds_.push_back({fd, POLLIN, 0});
  listener_fd_index_ = poll_fds_.size() - 1;
}
bool poll_manager::new_connection_available() const noexcept {
  return poll_fds_[listener_fd_index_].revents;
}
}  // namespace lib::connection