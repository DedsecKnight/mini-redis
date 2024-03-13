#pragma once

#include <include/connection/listener.h>

#include <list>
#include <string_view>
#include <unordered_map>

#include "include/connection/connection.h"
#include "include/connection/poll_manager.h"
#include "include/data/global_data.h"
#include "include/interface/server.h"
#include "include/protocol/request.h"
#include "include/protocol/response.h"

namespace mini_redis {

namespace libcon = lib::connection;

class server : public lib::interface::server {
 public:
  static constexpr const int IDLE_TIMEOUT_MS = 5 * 1000;
  server(std::string_view hostname, std::string_view port);
  void run() noexcept;
  void enable_nonblocking_io() const noexcept;
  virtual void on_request_available_cb(
      const lib::protocol::request& request,
      libcon::connection& conn) noexcept override;
  lib::protocol::response process_request(
      const lib::protocol::request& request) noexcept;

 private:
  void register_new_connection(libcon::connection& new_connection);
  void find_and_process_idle_connections() noexcept;
  uint32_t calculate_poll_timeout() const noexcept;
  using conn_idle_timer_t = std::pair<int, uint64_t>;
  using sock_fd_t = int;

 private:
  void register_conn_idle_timer(const sock_fd_t sock_id) noexcept;
  void re_register_conn_idle_timer(const sock_fd_t sock_id) noexcept;
  void unregister_conn_idle_timer(const sock_fd_t sock_id) noexcept;

 private:
  std::string_view hostname_, port_;
  libcon::listener listener_;
  libcon::poll_manager poll_manager_;
  std::vector<libcon::connection> client_connections_;
  data::global_data data_bank_{};
  std::list<conn_idle_timer_t> idle_list_;
  std::unordered_map<sock_fd_t, decltype(idle_list_)::iterator>
      conn_to_iterator_;
};
}  // namespace mini_redis