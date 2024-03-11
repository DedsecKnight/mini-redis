#pragma once

#include <include/connection/listener.h>

#include <string_view>

namespace mini_redis {

namespace libcon = lib::connection;

class server {
 public:
  server(std::string_view hostname, std::string_view port);
  void run() const noexcept;

 private:
  std::string_view hostname_, port_;
  libcon::listener listener_;
};
}  // namespace mini_redis