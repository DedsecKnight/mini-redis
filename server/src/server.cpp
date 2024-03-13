#include <include/server.h>
#include <sys/socket.h>

#include <cassert>
#include <cstdio>
#include <cstdlib>

#include "include/commands/del.h"
#include "include/commands/get.h"
#include "include/commands/set.h"
#include "include/connection/connection.h"
#include "include/protocol/request.h"
#include "include/protocol/response.h"
#include "include/time/manager.h"

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
    if (poll_manager_.poll(calculate_poll_timeout()) < 0) {
      exit(1);
    }
    poll_manager_.process_active_connection([&](int fd) -> void {
      auto& conn = client_connections_[fd];
      assert(conn_to_iterator_.find(fd) != conn_to_iterator_.end());
      auto conn_idle_timer = *conn_to_iterator_[fd];
      conn_idle_timer.second = lib::time::get_monotonic_usec();
      idle_list_.erase(conn_to_iterator_[fd]);
      idle_list_.push_front(conn_idle_timer);
      conn_to_iterator_[fd] = idle_list_.begin();
      if (conn.get_state() == connection_state::initialized) {
        conn.set_state(connection_state::reading);
      }
      conn.process_connection();
      if (conn.get_state() == connection_state::ended) {
        idle_list_.erase(conn_to_iterator_[fd]);
        conn_to_iterator_.erase(fd);
        conn.destroy();
      }
    });
    find_and_process_idle_connections();
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
  conn_idle_timer_t conn_idle_timer{new_connection.get_id(),
                                    lib::time::get_monotonic_usec()};
  idle_list_.push_front(conn_idle_timer);
  conn_to_iterator_[new_connection.get_id()] = idle_list_.begin();
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
void server::find_and_process_idle_connections() noexcept {
  auto current_us = lib::time::get_monotonic_usec();
  while (!idle_list_.empty()) {
    auto curr_element = *idle_list_.begin();
    auto next_us = curr_element.second + IDLE_TIMEOUT_MS * 1000;
    if (next_us >= current_us + 1000) {
      break;
    }
    printf("removing idle connection at fd = %d\n", curr_element.first);
    idle_list_.erase(idle_list_.begin());
    conn_to_iterator_.erase(curr_element.first);
    client_connections_[curr_element.first].destroy();
  }
}
uint32_t server::calculate_poll_timeout() const noexcept {
  if (idle_list_.empty()) {
    return 10000;
  }
  uint64_t current_us = lib::time::get_monotonic_usec();
  uint64_t next_us = idle_list_.begin()->second + IDLE_TIMEOUT_MS * 1000;
  if (next_us <= current_us) {
    return 0;
  }
  return static_cast<uint32_t>((next_us - current_us) / 1000);
}
}  // namespace mini_redis