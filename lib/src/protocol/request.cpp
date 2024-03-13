#include "include/protocol/request.h"

#include <cassert>
#include <memory>
#include <string>

namespace lib::protocol {
int request::size() const noexcept { return sz; }
void request::add_message(message&& msg) noexcept {
  msgs.push_back(std::move(msg));
  num_msg++;
  sz += msgs.back().msg_size + sizeof(int);
}
std::string request::to_string() const noexcept {
  int total_size = 0;
  for (int i = 0; i < num_msg; i++) {
    total_size += msgs[i].msg_size + (i != num_msg - 1);
  }
  std::string ret{};
  for (int i = 0, ptr = 0; i < num_msg; i++, ptr += msgs[i].msg_size) {
    ret.append(msgs[i].msg_content);
    if (i != num_msg - 1) {
      ret += ',';
      ptr++;
    }
  }
  return ret;
}
int request::num_messages() const noexcept { return num_msg; }
std::unique_ptr<char> request::serialize() const noexcept {
  char* ret = new char[sz];
  memset(ret, 0, sz);
  memcpy(ret, &num_msg, sizeof(num_msg));
  int total_copied = sizeof(num_msg);
  for (int i = 0, ptr = 4; i < num_msg; i++) {
    memcpy(ret + ptr, &msgs[i].msg_size, sizeof(msgs[i].msg_size));
    memcpy(ret + ptr + sizeof(msgs[i].msg_size), msgs[i].msg_content,
           static_cast<size_t>(msgs[i].msg_size));
    ptr += sizeof(msgs[i].msg_size) + msgs[i].msg_size;
    total_copied += sizeof(msgs[i].msg_size) + msgs[i].msg_size;
  }
  assert(total_copied == sz);
  return std::unique_ptr<char>{ret};
}
void request::add_message(const message& msg) noexcept {
  msgs.push_back(msg);
  num_msg++;
  sz += msgs.back().msg_size + sizeof(int);
}
message const& request::get_msg(size_t index) const noexcept {
  return msgs[index];
}
}  // namespace lib::protocol
