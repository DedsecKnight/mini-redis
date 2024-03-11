#include <include/server.h>
#include <sys/socket.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>

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
    std::string_view msg{"hello"};
    if (new_conn->send(msg.data(), msg.size()) == -1) {
      exit(1);
    }
    char buf[4096];
    memset(buf, 0, sizeof(buf));
    if (new_conn->receive(buf, 5) == -1) {
      exit(1);
    }
    printf("from client: %s\n", buf);
  }
}
}  // namespace mini_redis