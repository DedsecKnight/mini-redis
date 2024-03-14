#include "include/data_types/redis_nil.h"

#include <cstring>
#include <memory>

#include "include/data_types/redis_type.h"

namespace lib::data_types {
std::pair<redis_nil::size_type, std::unique_ptr<char>> redis_nil::serialize()
    const noexcept {
  char* ret = new char[sizeof(TYPE_CODE)];
  memcpy(ret, &TYPE_CODE, sizeof(TYPE_CODE));
  return std::make_pair(sizeof(TYPE_CODE), std::unique_ptr<char>{ret});
}
std::string redis_nil::to_string() const noexcept {
  return std::string{"(nil)"};
}
redis_nil redis_nil::from(const char* buffer) noexcept { return redis_nil{}; }
std::unique_ptr<redis_type> redis_nil::clone() const noexcept {
  return std::make_unique<redis_nil>();
}
int redis_nil::raw_size() const noexcept { return sizeof(TYPE_CODE); }
}  // namespace lib::data_types