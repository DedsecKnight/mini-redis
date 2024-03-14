#include "include/protocol/response.h"

#include <cstring>
#include <memory>

namespace lib::protocol {
int response::size() const noexcept { return sz_; }
std::string response::to_string() const noexcept {
  std::string ret{"{ "};
  switch (code_) {
    case lib::protocol::response_code::err: {
      ret += "ERR";
      break;
    }
    case lib::protocol::response_code::nx: {
      ret += "NOT_EXISTS";
      break;
    }
    case lib::protocol::response_code::ok: {
      ret += "OK";
      break;
    }
  }
  ret += ", ";
  ret += data_->to_string();
  ret += " }";
  return ret;
}
std::pair<int, std::unique_ptr<char>> response::serialize() const noexcept {
  char* ret = new char[sizeof(response_code) + sz_ + sizeof(sz_)];
  memcpy(ret, &sz_, sizeof(sz_));
  memcpy(&ret[sizeof(sz_)], &code_, sizeof(code_));
  auto [data_size, raw_data] = data_->serialize();
  memcpy(&ret[sizeof(code_) + sizeof(sz_)], raw_data.get(), data_size);
  return std::make_pair(sizeof(int) + sz_, std::unique_ptr<char>{ret});
}
response& response::operator=(const response& other) {
  code_ = other.code_;
  sz_ = other.sz_;
  data_ = other.data_->clone();
  return *this;
}
response::response(const response& other) {
  code_ = other.code_;
  sz_ = other.sz_;
  data_ = other.data_->clone();
}
response& response::operator=(response&& other) {
  code_ = std::move(other.code_);
  sz_ = std::move(other.sz_);
  data_ = std::move(other.data_);
  return *this;
}
response::response(response&& other) {
  code_ = std::move(other.code_);
  sz_ = std::move(other.sz_);
  data_ = std::move(other.data_);
}
}  // namespace lib::protocol