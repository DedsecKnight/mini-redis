#pragma once

#include "include/data_types/redis_type.h"
namespace lib::data_types {
class registry {
 private:
  static constexpr const int NIL_CODE = 0;
  static constexpr const int STRING_CODE = 1;

 public:
  static std::unique_ptr<redis_type> deserialize(char* buffer) noexcept;
};
}  // namespace lib::data_types