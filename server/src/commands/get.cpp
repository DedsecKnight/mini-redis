#include "include/commands/get.h"

namespace mini_redis::commands {
lib::protocol::response get_command::execute(
    const data::global_data &data_bank,
    const lib::protocol::request &req) noexcept {
  if (req.num_messages() != 2) {
    char *err_msg;
    sprintf(err_msg, "invalid number of arguments. expected 2, %d found",
            req.num_messages());
    return lib::protocol::response{lib::protocol::response_code::err,
                                   std::string{err_msg}};
  }
  auto key = std::string{req.get_msg(1).msg_content};
  auto value_opt = data_bank.get(key);
  if (!value_opt.has_value()) {
    return lib::protocol::response{lib::protocol::response_code::nx, nullptr};
  }
  return lib::protocol::response{lib::protocol::response_code::ok,
                                 value_opt.value()};
}
}  // namespace mini_redis::commands