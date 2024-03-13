
#include "include/commands/del.h"

namespace mini_redis::commands {
lib::protocol::response del_command::execute(
    data::global_data &data_bank, const lib::protocol::request &req) noexcept {
  if (req.num_messages() != 2) {
    char *err_msg;
    sprintf(err_msg, "invalid number of arguments. expected 2, %d found",
            req.num_messages());
    return lib::protocol::response{lib::protocol::response_code::err,
                                   std::string{err_msg}};
  }
  auto key = std::string{req.get_msg(1).msg_content};
  auto rv = data_bank.del(key);
  if (!rv) {
    return lib::protocol::response{lib::protocol::response_code::nx,
                                   "key not found"};
  }
  return lib::protocol::response{lib::protocol::response_code::ok, ""};
}
}  // namespace mini_redis::commands