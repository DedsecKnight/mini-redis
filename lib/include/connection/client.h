#pragma once

#include <sys/socket.h>

#include <string_view>

#include "include/connection/connection.h"

namespace lib::connection {
class client : public connection {
 public:
  client() = default;
  int connect_to(std::string_view hostname, std::string_view port);

  client(const client&) = delete;
  client& operator=(const client&) = delete;

  client(client&&);
  client& operator=(client&&);
};
}  // namespace lib::connection