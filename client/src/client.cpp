#include "include/client.h"

#include <cstdio>
#include <cstdlib>

#include "include/protocol/message.h"
#include "include/protocol/request.h"

namespace mini_redis {
int client::connect_to(std::string_view hostname, std::string_view port) {
  return client_.connect_to(hostname, port);
}

void client::run() const noexcept {
  lib::protocol::message msg1{std::string_view{"hello"}};
  lib::protocol::message msg2{std::string_view{"world"}};
  lib::protocol::message msg3{std::string_view{"something different"}};
  lib::protocol::request req;
  req.add_message(std::move(msg1));
  req.add_message(std::move(msg2));
  req.add_message(std::move(msg3));
  if (client_.send_request(req) == -1) {
    fprintf(stderr, "something is wrong\n");
    exit(1);
  }
  auto response = client_.get_next_request();
  if (!response.has_value()) {
    fprintf(stderr, "error receiving data from server");
    exit(1);
  }
  printf("from server [%d messages, %d bytes]: %s\n", response->num_messages(),
         response->size(), response->to_string().data());
}
}  // namespace mini_redis