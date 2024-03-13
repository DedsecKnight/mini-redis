#pragma once

#include <memory>
#include <string>
namespace lib::protocol {

enum class response_code { ok = 0, err = 1, nx = 2 };

class response {
 public:
  explicit response(response_code code, const std::string& data);
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
  char data_[128];
};
}  // namespace lib::protocol