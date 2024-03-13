#include "include/data/global_data.h"
#include "include/protocol/request.h"
#include "include/protocol/response.h"

namespace mini_redis::commands {
class set_command {
 public:
  static lib::protocol::response execute(
      data::global_data& data_bank,
      const lib::protocol::request& request) noexcept;
};
}  // namespace mini_redis::commands