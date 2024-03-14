#include "include/data_types/registry.h"

#include <cassert>
#include <cstring>

#include "include/data_types/redis_array.h"
#include "include/data_types/redis_int.h"
#include "include/data_types/redis_nil.h"
#include "include/data_types/redis_string.h"
#include "include/data_types/redis_type.h"

namespace lib::data_types {
std::unique_ptr<redis_type> registry::deserialize(char* buffer) noexcept {
  int type_code;
  memcpy(&type_code, buffer, sizeof(type_code));
  switch (type_code) {
    case NIL_CODE: {
      return std::make_unique<redis_nil>();
    }
    case STRING_CODE: {
      return std::make_unique<redis_string>(
          redis_string::from(&buffer[sizeof(int)]));
    }
    case INT_CODE: {
      return std::make_unique<redis_int>(redis_int::from(&buffer[sizeof(int)]));
    }
    case ARRAY_CODE: {
      return std::make_unique<redis_array>(
          redis_array::from(&buffer[sizeof(int)]));
    }
    default: {
      assert(false);
      return nullptr;
    }
  }
}
}  // namespace lib::data_types