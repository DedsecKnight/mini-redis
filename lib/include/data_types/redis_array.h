#pragma once

#include <cassert>
#include <memory>
#include <vector>

#include "include/data_types/redis_double.h"
#include "include/data_types/redis_int.h"
#include "include/data_types/redis_string.h"
#include "include/data_types/redis_type.h"

namespace lib::data_types {
class redis_array : public redis_type {
 public:
  static constexpr const int TYPE_CODE = 3;
  std::pair<size_type, std::unique_ptr<char>> serialize()
      const noexcept override;
  std::string to_string() const noexcept override;
  static redis_array from(char* buffer) noexcept;
  template <typename T>
  static redis_array from(const std::vector<T>& data) noexcept {
    redis_array ret;
    ret.elements_sz_ = 0;
    for (const auto& elem : data) {
      if constexpr (std::is_same_v<T, std::string>) {
        ret.data_.push_back(
            std::make_unique<redis_string>(redis_string::from(elem)));
      } else if constexpr (std::is_same_v<T, int64_t>) {
        ret.data_.push_back(std::make_unique<redis_int>(redis_int::from(elem)));
      } else if constexpr (std::is_same_v<T, double>) {
        ret.data_.push_back(
            std::make_unique<redis_double>(redis_double::from(elem)));
      } else {
        assert(false);
      }
      ret.elements_sz_ += ret.data_.back()->raw_size();
    }
    return ret;
  }
  int raw_size() const noexcept override;
  std::unique_ptr<redis_type> clone() const noexcept override;

 private:
  int elements_sz_{-1};
  std::vector<std::unique_ptr<redis_type>> data_;
};

}  // namespace lib::data_types