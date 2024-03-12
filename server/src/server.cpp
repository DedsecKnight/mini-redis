#include <include/server.h>
#include <sys/socket.h>

#include <cstdio>
#include <cstdlib>

#include "include/connection/connection.h"

namespace mini_redis {
server::server(std::string_view hostname, std::string_view port)
    : hostname_{hostname}, port_{port}, listener_{hostname, port} {}
void server::run() noexcept {
  if (listener_.listen() < 0) {
    exit(0);
  }
  using connection_state = libcon::connection::connection_state;
  printf("Listening to %s:%s\n", hostname_.data(), port_.data());
  while (true) {
    poll_manager_.reset_args();
    listener_.register_listener_to_poll_manager(poll_manager_);
    for (const auto& conn : client_connections_) {
      if (conn.get_state() == connection_state::ended ||
          conn.get_state() == connection_state::uninitialized) {
        continue;
      }
      conn.register_self_to_poll_manager(poll_manager_);
    }
    if (poll_manager_.poll(1000) < 0) {
      exit(1);
    }
    poll_manager_.process_active_connection([&](int fd) -> void {
      auto& conn = client_connections_[fd];
      if (conn.get_state() == connection_state::initialized) {
        conn.set_state(connection_state::reading);
      }
      conn.process_connection();
      if (conn.get_state() == connection_state::ended) {
        conn.destroy();
      }
    });
    if (poll_manager_.new_connection_available()) {
      auto new_conn = listener_.accept_new_listener();
      if (new_conn != std::nullopt) {
        printf("accepted new connection\n");
        register_new_connection(new_conn.value());
      }
    }
  }
}
void server::enable_nonblocking_io() const noexcept {
  if (listener_.enable_nonblocking_io() == -1) {
    exit(0);
  }
}
void server::register_new_connection(libcon::connection& new_connection) {
  new_connection.enable_nonblocking_io();
  auto connection_id = static_cast<size_t>(new_connection.get_id());
  if (connection_id >= client_connections_.size()) {
    client_connections_.resize(connection_id + 1);
  }
  client_connections_[connection_id] = std::move(new_connection);
}
}  // namespace mini_redis