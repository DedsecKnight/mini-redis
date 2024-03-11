#pragma once

#include <include/connection/client.h>

namespace mini_redis {

namespace libcon = lib::connection;

class client {
 public:
  int connect_to(std::string_view hostname, std::string_view port);
  void run() const noexcept;

 private:
  libcon::client client_;
};
}  // namespace mini_redis