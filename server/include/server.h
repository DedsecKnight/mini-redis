#pragma once

#include <include/connection/listener.h>

#include <string_view>

#include "include/connection/connection.h"
#include "include/connection/poll_manager.h"

namespace mini_redis {

namespace libcon = lib::connection;

class server {
 public:
  server(std::string_view hostname, std::string_view port);
  void run() noexcept;
  void enable_nonblocking_io() const noexcept;

 private:
  void register_new_connection(libcon::connection& new_connection);

 private:
  std::string_view hostname_, port_;
  libcon::listener listener_;
  libcon::poll_manager poll_manager_;
  std::vector<libcon::connection> client_connections_;
};
}  // namespace mini_redis