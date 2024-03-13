#include "include/commands/ttl.h"

#include <cmath>

#include "include/protocol/response.h"
#include "include/time/manager.h"

namespace mini_redis::commands {
lib::protocol::response ttl_command::execute(
    const data::global_data &data_bank, const lib::protocol::request &req) {
  if (req.num_messages() != 2) {
    char *err_msg;
    sprintf(err_msg, "invalid number of arguments. expected 2, %d found",
            req.num_messages());
    return lib::protocol::response{lib::protocol::response_code::err,
                                   std::string{err_msg}};
  }
  auto key = std::string{req.get_msg(1).msg_content};
  auto [response_code, key_ttl] = data_bank.get_ttl_ts(key);
  if (response_code == -2) {
    return lib::protocol::response{lib::protocol::response_code::nx,
                                   "key not found"};
  }
  if (response_code == -1) {
    return lib::protocol::response{lib::protocol::response_code::nx,
                                   "no expiration associated with key"};
  }
  return lib::protocol::response{
      lib::protocol::response_code::ok,
      std::to_string(static_cast<uint32_t>(
          std::round(static_cast<double>(static_cast<uint32_t>(
                         (key_ttl - lib::time::get_monotonic_usec()) / 1000)) /
                     1000)))};
}
}  // namespace mini_redis::commands