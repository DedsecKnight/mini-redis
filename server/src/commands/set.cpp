#include "include/commands/set.h"

#include "include/protocol/response.h"

namespace mini_redis::commands {
lib::protocol::response set_command::execute(
    data::global_data &data_bank, const lib::protocol::request &req) noexcept {
  if (req.num_messages() != 3) {
    char *err_msg;
    sprintf(err_msg, "invalid number of arguments. expected 3, %d found",
            req.num_messages());
    return lib::protocol::response{lib::protocol::response_code::err,
                                   std::string{err_msg}};
  }
  auto key = std::string{req.get_msg(1).msg_content};
  auto value = std::string{req.get_msg(2).msg_content};
  data_bank.set(key, value);
  return lib::protocol::response{lib::protocol::response_code::ok, "success"};
}
}  // namespace mini_redis::commands