#include "include/commands/expire.h"

namespace mini_redis::commands {
lib::protocol::response expire_command::execute(
    data::global_data& data_bank, const lib::protocol::request& req) noexcept {
  if (req.num_messages() != 3) {
    char* err_msg;
    sprintf(err_msg, "invalid number of arguments. expected 3, %d found",
            req.num_messages());
    return lib::protocol::response{lib::protocol::response_code::err,
                                   std::string{err_msg}};
  }
  auto key = std::string{req.get_msg(1).msg_content};
  if (!data_bank.get(key).has_value()) {
    return lib::protocol::response{lib::protocol::response_code::nx,
                                   "key not found"};
  }
  try {
    int64_t ttl_ms = std::stol(std::string{req.get_msg(2).msg_content}) * 1000;
    data_bank.set_ttl_ms(key, ttl_ms);
    return lib::protocol::response{lib::protocol::response_code::ok, ""};
  } catch (const std::invalid_argument& ex) {
    return lib::protocol::response{lib::protocol::response_code::err,
                                   ex.what()};
  }
}
}  // namespace mini_redis::commands