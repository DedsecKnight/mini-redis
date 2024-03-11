#include "include/connection/client.h"

#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstdio>
#include <cstring>

#include "include/connection/connection.h"

namespace lib::connection {
int client::connect_to(std::string_view hostname, std::string_view port) {
  addrinfo hints, *addrlist, *ptr;
  memset(&hints, 0, sizeof(hints));
  memset(&sock_addr_, 0, sizeof(sock_addr_));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  int status = getaddrinfo(hostname.data(), port.data(), &hints, &addrlist);
  if (status != 0) {
    fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
    return -1;
  }

  for (ptr = addrlist; ptr != nullptr; ptr = ptr->ai_next) {
    sockfd_ = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
    if (sockfd_ < 0) {
      perror("socket");
      continue;
    }
    if (connect(sockfd_, ptr->ai_addr, ptr->ai_addrlen) == -1) {
      close(sockfd_);
      perror("connect");
      continue;
    }
    memcpy(&sock_addr_, ptr->ai_addr, ptr->ai_addrlen);
    break;
  }
  freeaddrinfo(addrlist);
  if (ptr == nullptr) {
    fprintf(stderr, "error connecting to server\n");
    return -1;
  }
  state_ = connection::connection_state::initialized;
  return 0;
}

client::client(client&& other) {
  sockfd_ = std::move(other.sockfd_);
  sock_addr_ = std::move(other.sock_addr_);
  state_ = std::move(other.state_);
}

client& client::operator=(client&& other) {
  sockfd_ = std::move(other.sockfd_);
  sock_addr_ = std::move(other.sock_addr_);
  state_ = std::move(other.state_);
  return *this;
}
}  // namespace lib::connection