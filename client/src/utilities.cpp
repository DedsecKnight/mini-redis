#include "include/protocol/message.h"
#include "include/protocol/request.h"

namespace mini_redis::utilities {
template <typename... ArgTs>
lib::protocol::request construct_request(ArgTs... msgs) {
  lib::protocol::request req;
  (req.add_message(lib::protocol::message{msgs}), ...);
  return req;
}
}  // namespace mini_redis::utilities
