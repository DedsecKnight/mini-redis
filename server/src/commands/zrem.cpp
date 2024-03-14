
#include "include/commands/zrem.h"

#include <string>

#include "include/protocol/response.h"

namespace mini_redis::commands {
lib::protocol::response zrem_command::execute(
    data::global_data &data_bank, const lib::protocol::request &req) noexcept {
  if (req.num_messages() != 3) {
    char *err_msg;
    sprintf(err_msg, "invalid number of arguments. expected 3, %d found",
            req.num_messages());
    return lib::protocol::response{lib::protocol::response_code::err,
                                   std::string{err_msg}};
  }
  auto sorted_set_key = std::string{req.get_msg(1).msg_content};
  auto member_name = std::string{req.get_msg(2).msg_content};
  data_bank.sorted_set_erase_member(sorted_set_key, member_name);
  return lib::protocol::response{lib::protocol::response_code::ok, ""};
}

}  // namespace mini_redis::commands