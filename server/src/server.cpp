#include <include/server.h>
#include <sys/socket.h>

#include <cstdio>
#include <cstdlib>

#include "include/commands/del.h"
#include "include/commands/get.h"
#include "include/commands/set.h"
#include "include/connection/connection.h"
#include "include/protocol/request.h"
#include "include/protocol/response.h"

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
        new_conn->register_self_to_server(this);
        register_new_connection(new_conn.value());
        printf("accepted new connection\n");
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
void server::on_request_available_cb(const lib::protocol::request& request,
                                     libcon::connection& conn) noexcept {
  std::string req_str = request.to_string();
  printf("received %d messages from client: %s\n", request.num_messages(),
         req_str.data());
  // generate response
  auto response = process_request(request);
  auto raw_response = response.serialize();
  printf("responding to client with: %s\n", response.to_string().data());
  // send response back to connection
  conn.consume_buffer(request.size());
  conn.nonblocking_send(raw_response.get(), response.size());
}
lib::protocol::response server::process_request(
    const lib::protocol::request& request) noexcept {
  const auto& first_msg = request.get_msg(0);
  if (!strcmp(first_msg.msg_content, "get")) {
    return commands::get_command::execute(data_bank_, request);
  }
  if (!strcmp(first_msg.msg_content, "set")) {
    return commands::set_command::execute(data_bank_, request);
  }
  if (!strcmp(first_msg.msg_content, "del")) {
    return commands::del_command::execute(data_bank_, request);
  }
  return lib::protocol::response{lib::protocol::response_code::err,
                                 "unknown command found"};
}
}  // namespace mini_redis