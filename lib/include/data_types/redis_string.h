#pragma once

#include <memory>

#include "include/data_types/redis_type.h"

namespace lib::data_types {
class redis_string : public redis_type {
 public:
  static constexpr const int TYPE_CODE = 1;
  virtual std::pair<size_type, std::unique_ptr<char>> serialize()
      const noexcept override;
  virtual std::string to_string() const noexcept override;
  static redis_string from(std::string_view data) noexcept;
  static redis_string from(char* raw_buffer) noexcept;
  virtual int raw_size() const noexcept override;
  virtual std::unique_ptr<redis_type> clone() const noexcept override;

 private:
  int sz_;
  std::string data_;
};
}  // namespace lib::data_types