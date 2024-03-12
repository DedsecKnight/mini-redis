#include "include/connection/listener.h"

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstring>
#include <optional>
#include <string_view>
#include <utility>

#include "include/connection/connection.h"

namespace lib::connection {
listener::listener(std::string_view hostname, std::string_view port) {
  addrinfo hints, *addrlist, *ptr;
  memset(&hints, 0, sizeof(hints));
  memset(&sock_addr_, 0, sizeof(sock_addr_));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  int status = getaddrinfo(hostname.data(), port.data(), &hints, &addrlist);
  if (status != 0) {
    fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
    exit(1);
  }

  // find an address that can be binded with
  for (ptr = addrlist; ptr != nullptr; ptr = ptr->ai_next) {
    sockfd_ = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
    if (sockfd_ < 0) {
      perror("socket");
      continue;
    }
    int yes = 1;
    if (setsockopt(sockfd_, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) ==
        -1) {
      perror("setsockopt");
      exit(1);
    }
    if (bind(sockfd_, ptr->ai_addr, ptr->ai_addrlen) == -1) {
      close(sockfd_);
      perror("bind");
      continue;
    }
    // record address
    memcpy(&sock_addr_, ptr->ai_addr, ptr->ai_addrlen);
    break;
  }
  freeaddrinfo(addrlist);
  if (ptr == nullptr) {
    fprintf(stderr, "error connection to address\n");
    exit(1);
  }
  state_ = connection::connection_state::initialized;
}

listener::listener(listener&& other) {
  sockfd_ = std::move(other.sockfd_);
  sock_addr_ = std::move(other.sock_addr_);
  state_ = std::move(other.state_);
}

listener& listener::operator=(listener&& other) {
  sockfd_ = std::move(other.sockfd_);
  sock_addr_ = std::move(other.sock_addr_);
  state_ = std::move(other.state_);
  return *this;
}

int listener::listen() const noexcept {
  if (::listen(sockfd_, MAX_CONNECTION_ALLOWED) == -1) {
    perror("listen");
    return -1;
  }
  return 0;
}

listener::listener(int sockfd, sockaddr_storage&& addr) {
  sockfd_ = sockfd;
  sock_addr_ = std::move(addr);
  state_ = connection_state::initialized;
}

std::optional<connection> listener::accept_new_listener() const noexcept {
  sockaddr_storage client_addr;
  socklen_t socklen = sizeof(client_addr);
  int new_fd;
  if ((new_fd = accept(sockfd_, (sockaddr*)&client_addr, &socklen)) == -1) {
    perror("accept");
    return std::nullopt;
  }
  return std::optional<connection>{std::in_place, new_fd,
                                   std::move(client_addr)};
}
void listener::register_listener_to_poll_manager(
    poll_manager& manager) noexcept {
  if (state_ == connection_state::uninitialized ||
      state_ == connection_state::ended) {
    return;
  }
  manager.register_listener_cb(sockfd_);
}
}  // namespace lib::connection