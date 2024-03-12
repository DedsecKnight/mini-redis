#pragma once

#include <memory>
#include <vector>

#include "include/protocol/message.h"
namespace lib::protocol {
class request {
 public:
  int size() const noexcept;
  void add_message(message&& msg) noexcept;
  void add_message(const message& msg) noexcept;
  std::string to_string() const noexcept;
  std::unique_ptr<char> serialize() const noexcept;
  int num_messages() const noexcept;

 private:
  int num_msg{0}, sz{sizeof(int)};
  std::vector<message> msgs;
};
}  // namespace lib::protocol