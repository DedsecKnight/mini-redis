#pragma once

#include <cassert>
#include <memory>
#include <string>
#include <type_traits>

#include "include/data_types/redis_double.h"
#include "include/data_types/redis_nil.h"
#include "include/data_types/redis_string.h"
#include "include/data_types/redis_type.h"

namespace lib::protocol {

enum class response_code { ok = 0, err = 1, nx = 2 };

class response {
 public:
  explicit response(response_code code,
                    std::unique_ptr<data_types::redis_type>&& data)
      : code_{code},
        sz_{static_cast<int>(sizeof(code) + data->raw_size())},
        data_{std::move(data)} {}
  template <typename T>
  explicit response(response_code code, const T& data) : code_{code} {
    if constexpr (std::is_same_v<T, std::string_view> ||
                  std::is_same_v<T, std::string>) {
      data_ = std::make_unique<data_types::redis_string>(
          data_types::redis_string::from(data));
    } else if constexpr (std::is_same_v<T, decltype(nullptr)>) {
      data_ = std::make_unique<data_types::redis_nil>(data_types::redis_nil{});
    } else if constexpr (std::is_same_v<T, double>) {
      data_ = std::make_unique<data_types::redis_double>(
          data_types::redis_double::from(data));
    } else {
      assert(false);
    }
    sz_ = sizeof(response_code) + data_->raw_size();
  }
  explicit response(response_code code, const char* data) : code_{code} {
    data_ = std::make_unique<data_types::redis_string>(
        data_types::redis_string::from(std::string_view{data}));
    sz_ = sizeof(response_code) + data_->raw_size();
  }
  response& operator=(const response&);
  response(const response&);
  response& operator=(response&&);
  response(response&&);

  int size() const noexcept;
  std::string to_string() const noexcept;
  std::pair<int, std::unique_ptr<char>> serialize() const noexcept;

 private:
  response_code code_;
  int sz_;
  std::unique_ptr<data_types::redis_type> data_{nullptr};
};
}  // namespace lib::protocol