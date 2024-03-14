
#include "include/data_types/redis_double.h"

#include <cstring>
#include <memory>
#include <string>

namespace lib::data_types {
std::pair<redis_type::size_type, std::unique_ptr<char>>
redis_double::serialize() const noexcept {
  int raw_size = sizeof(TYPE_CODE) + sizeof(data_);
  char* ret = new char[raw_size];
  memcpy(ret, &TYPE_CODE, sizeof(TYPE_CODE));
  memcpy(&ret[sizeof(TYPE_CODE)], &data_, sizeof(data_));
  return std::make_pair(raw_size, std::unique_ptr<char>{ret});
}
std::string redis_double::to_string() const noexcept {
  return "double { " + std::to_string(data_) + " }";
}
int redis_double::raw_size() const noexcept {
  return sizeof(TYPE_CODE) + sizeof(data_);
}
redis_double redis_double::from(char* buffer) noexcept {
  redis_double ret;
  memcpy(&ret.data_, buffer, sizeof(data_));
  return ret;
}
redis_double redis_double::from(double value) noexcept {
  redis_double ret;
  ret.data_ = value;
  return ret;
}
std::unique_ptr<redis_type> redis_double::clone() const noexcept {
  return std::make_unique<redis_double>(redis_double::from(data_));
}
}  // namespace lib::data_types