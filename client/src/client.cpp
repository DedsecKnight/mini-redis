#include "include/client.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>

namespace mini_redis {
int client::connect_to(std::string_view hostname, std::string_view port) {
  return client_.connect_to(hostname, port);
}

void client::run() const noexcept {
  char buf[4096];
  memset(buf, 0, sizeof(buf));
  if (client_.receive(buf, 5) == -1) {
    exit(1);
  }
  printf("from server: %s\n", buf);
  std::string_view msg{"world"};
  if (client_.send(msg.data(), msg.size()) == -1) {
    exit(1);
  }
}
}  // namespace mini_redis