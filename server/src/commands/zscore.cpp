#include "include/commands/zscore.h"

#include "include/protocol/response.h"

namespace mini_redis::commands {
lib::protocol::response zscore_command::execute(
    const data::global_data &data_bank,
    const lib::protocol::request &req) noexcept {
  if (req.num_messages() != 3) {
    char *err_msg;
    sprintf(err_msg, "invalid number of arguments. expected 3, %d found",
            req.num_messages());
    return lib::protocol::response{lib::protocol::response_code::err,
                                   std::string{err_msg}};
  }
  auto sorted_set_key = std::string{req.get_msg(1).msg_content};
  auto member_name = std::string{req.get_msg(2).msg_content};
  auto score_opt =
      data_bank.sorted_set_query_member_score(sorted_set_key, member_name);
  if (!score_opt.has_value()) {
    return lib::protocol::response{lib::protocol::response_code::nx, nullptr};
  }
  return lib::protocol::response{lib::protocol::response_code::ok,
                                 score_opt.value()};
}
}  // namespace mini_redis::commands