#include <include/server.h>
#include <sys/socket.h>

#include <cstdio>
#include <cstdlib>

namespace mini_redis {
server::server(std::string_view hostname, std::string_view port)
    : hostname_{hostname}, port_{port}, listener_{hostname, port} {}
void server::run() const noexcept {
  if (listener_.listen() < 0) {
    exit(0);
  }
  printf("Listening to %s:%s\n", hostname_.data(), port_.data());
  while (true) {
    auto new_conn = listener_.accept_new_listener();
    if (!new_conn.has_value()) {
      continue;
    }
    printf("accepted new connection\n");
    protocol::message msg{std::string_view{"hello"}};
    if (new_conn->send_msg(msg) == -1) {
      exit(1);
    }
    auto response = new_conn->get_next_msg();
    if (!response.has_value()) {
      exit(1);
    }
    printf("from client [%d bytes]: %s\n", response->msg_size,
           response->msg_content);
  }
}
}  // namespace mini_redis