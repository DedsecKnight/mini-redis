#pragma once

#include <memory>
#include <vector>

#include "include/data_types/redis_type.h"

namespace lib::data_types {
class redis_array : public redis_type {
 public:
  static constexpr const int TYPE_CODE = 3;
  virtual std::pair<size_type, std::unique_ptr<char>> serialize()
      const noexcept override;
  virtual std::string to_string() const noexcept override;
  static redis_array from(char* buffer) noexcept;
  virtual int raw_size() const noexcept override;
  virtual std::unique_ptr<redis_type> clone() const noexcept override;

 private:
  int elements_sz_{-1};
  std::vector<std::unique_ptr<redis_type>> data_;
};

}  // namespace lib::data_types