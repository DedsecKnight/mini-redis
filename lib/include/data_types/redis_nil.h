#pragma once

#include "include/data_types/redis_type.h"

namespace lib::data_types {
class redis_nil : public redis_type {
 public:
  static constexpr const int TYPE_CODE = 0;
  std::pair<size_type, std::unique_ptr<char>> serialize()
      const noexcept override;
  std::string to_string() const noexcept override;
  static redis_nil from(const char* buffer) noexcept;
  int raw_size() const noexcept override;
  std::unique_ptr<redis_type> clone() const noexcept override;
};
}  // namespace lib::data_types