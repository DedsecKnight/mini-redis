#pragma once

#include "include/protocol/request.h"

namespace lib {
namespace connection {
class connection;
}
namespace interface {

class server {
 public:
  virtual void on_request_available_cb(const protocol::request&,
                                       connection::connection&) noexcept = 0;
  virtual ~server() = default;
};
}  // namespace interface
}  // namespace lib