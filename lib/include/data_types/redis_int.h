#pragma once

#include "include/data_types/redis_type.h"

namespace lib::data_types {
class redis_int : public redis_type {
 public:
  static constexpr const int TYPE_CODE = 2;
  std::pair<size_type, std::unique_ptr<char>> serialize()
      const noexcept override;
  std::string to_string() const noexcept override;
   static redis_int from(char* buffer) noexcept;
   static redis_int from(int64_t value) noexcept;
  int raw_size() const noexcept override;
  std::unique_ptr<redis_type> clone() const noexcept override;

 private:
  int64_t data_;
};

}  // namespace lib::data_types