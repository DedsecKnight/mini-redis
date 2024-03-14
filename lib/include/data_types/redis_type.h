#pragma once

#include <memory>
namespace lib::data_types {
class redis_type {
 public:
  using size_type = int;

 public:
  virtual ~redis_type() = default;
  virtual std::pair<size_type, std::unique_ptr<char>> serialize()
      const noexcept = 0;
  virtual std::string to_string() const noexcept = 0;
  virtual int raw_size() const noexcept = 0;
  virtual std::unique_ptr<redis_type> clone() const noexcept = 0;
};
}  // namespace lib::data_types