#include "include/data_types/redis_string.h"

#include <cstring>

namespace lib::data_types {
std::pair<redis_type::size_type, std::unique_ptr<char>>
redis_string::serialize() const noexcept {
  int raw_size = sizeof(TYPE_CODE) + sizeof(sz_) + sz_;
  char* ret = new char[raw_size];
  memcpy(ret, &TYPE_CODE, sizeof(TYPE_CODE));
  memcpy(&ret[sizeof(TYPE_CODE)], &sz_, sizeof(sz_));
  memcpy(&ret[sizeof(TYPE_CODE) + sizeof(sz_)], data_.data(), sz_);
  return std::make_pair(raw_size, std::unique_ptr<char>{ret});
}
std::string redis_string::to_string() const noexcept {
  return "string { " + data_ + " }";
}
int redis_string::raw_size() const noexcept {
  return sz_ + sizeof(int) + sizeof(TYPE_CODE);
}
redis_string redis_string::from(std::string_view data) noexcept {
  redis_string ret;
  ret.data_ = std::string{data};
  ret.sz_ = data.size();
  return ret;
}
std::unique_ptr<redis_type> redis_string::clone() const noexcept {
  return std::make_unique<redis_string>(
      redis_string::from(std::string_view{data_}));
}
redis_string redis_string::from(char* raw_buffer) noexcept {
  int sz;
  std::string data;
  memcpy(&sz, raw_buffer, sizeof(int));
  data.append(&raw_buffer[sizeof(int)], static_cast<size_t>(sz));
  return from(std::string_view{data});
}
}  // namespace lib::data_types