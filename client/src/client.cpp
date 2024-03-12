#include "include/client.h"

#include <cstdio>
#include <cstdlib>

namespace mini_redis {
int client::connect_to(std::string_view hostname, std::string_view port) {
  return client_.connect_to(hostname, port);
}

void client::run() const noexcept {
  protocol::message msg{std::string_view{"world"}};
  if (client_.send_msg(msg) == -1) {
    exit(1);
  }
  auto response = client_.get_next_msg();
  if (!response.has_value()) {
    fprintf(stderr, "error receiving data from server");
    exit(1);
  }
  printf("from server [%d bytes]: %s\n", response->msg_size,
         response->msg_content);
}
}  // namespace mini_redis