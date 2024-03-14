#include "include/commands/zadd.h"

#include <string>

#include "include/protocol/response.h"

namespace mini_redis::commands {
lib::protocol::response zadd_command::execute(
    data::global_data &data_bank, const lib::protocol::request &req) noexcept {
  if (req.num_messages() != 4) {
    char *err_msg;
    sprintf(err_msg, "invalid number of arguments. expected 4, %d found",
            req.num_messages());
    return lib::protocol::response{lib::protocol::response_code::err,
                                   std::string{err_msg}};
  }
  auto sorted_set_key = std::string{req.get_msg(1).msg_content};
  auto member_name = std::string{req.get_msg(3).msg_content};
  double member_score = atof(req.get_msg(2).msg_content);
  if (data_bank.sorted_set_set_member_data(sorted_set_key, member_name,
                                           member_score) < 0) {
    return lib::protocol::response{
        lib::protocol::response_code::err,
        "key already existed, but not of sorted set type"};
  }
  return lib::protocol::response{lib::protocol::response_code::ok, ""};
}

}  // namespace mini_redis::commands