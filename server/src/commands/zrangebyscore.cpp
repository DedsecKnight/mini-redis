#include "include/commands/zrangebyscore.h"

#include "include/data/global_data.h"
#include "include/protocol/request.h"
#include "include/protocol/response.h"

namespace mini_redis::commands {
lib::protocol::response zrangebyscore_command::execute(
    const data::global_data& data_bank,
    const lib::protocol::request& req) noexcept {
  if (req.num_messages() != 4) {
    char* err_msg;
    sprintf(err_msg, "invalid number of arguments. expected 4, %d found",
            req.num_messages());
    return lib::protocol::response{lib::protocol::response_code::err,
                                   std::string{err_msg}};
  }
  std::string key = std::string{req.get_msg(1).msg_content};
  double min_score = atof(req.get_msg(2).msg_content);
  double max_score = atof(req.get_msg(3).msg_content);
  auto members = data_bank.sorted_set_find_member_within_score_range(
      key, min_score, max_score);
  return lib::protocol::response{lib::protocol::response_code::ok, members};
}
}  // namespace mini_redis::commands