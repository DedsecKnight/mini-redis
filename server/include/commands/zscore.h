#pragma once

#include "include/data/global_data.h"
#include "include/protocol/request.h"
#include "include/protocol/response.h"

namespace mini_redis::commands {
class zscore_command {
 public:
  static lib::protocol::response execute(
      const data::global_data& data_bank,
      const lib::protocol::request& req) noexcept;
};
}  // namespace mini_redis::commands