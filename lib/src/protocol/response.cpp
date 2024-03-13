#include "include/protocol/response.h"

#include <cstring>
#include <memory>

namespace lib::protocol {
int response::size() const noexcept { return sz_; }
response::response(response_code code, const std::string& data)
    : code_{code}, sz_{static_cast<int>(data.size() + sizeof(response_code))} {
  memset(data_, 0, sizeof(data_));
  memcpy(data_, data.data(), data.size());
}
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
  if (sz_ > sizeof(code_) + sizeof(sz_)) {
    ret += ", ";
  }
  ret.append(data_);
  ret += " }";
  return ret;
}
std::pair<int, std::unique_ptr<char>> response::serialize() const noexcept {
  char* ret = new char[sizeof(response_code) + sz_ + sizeof(sz_)];
  memcpy(ret, &sz_, sizeof(sz_));
  memcpy(&ret[sizeof(sz_)], &code_, sizeof(code_));
  memcpy(&ret[sizeof(code_) + sizeof(sz_)], data_, sz_);
  return std::make_pair(sizeof(int) + sz_, std::unique_ptr<char>{ret});
}
response& response::operator=(const response& other) {
  code_ = other.code_;
  sz_ = other.sz_;
  memset(data_, 0, sizeof(data_));
  memcpy(data_, other.data_, sizeof(data_));
  return *this;
}
response::response(const response& other) {
  code_ = other.code_;
  sz_ = other.sz_;
  memset(data_, 0, sizeof(data_));
  memcpy(data_, other.data_, sizeof(data_));
}
response& response::operator=(response&& other) {
  code_ = std::move(other.code_);
  sz_ = std::move(other.sz_);
  memset(data_, 0, sizeof(data_));
  memmove(data_, other.data_, sizeof(data_));
  return *this;
}
response::response(response&& other) {
  code_ = std::move(other.code_);
  sz_ = std::move(other.sz_);
  memset(data_, 0, sizeof(data_));
  memmove(data_, other.data_, sizeof(data_));
}
}  // namespace lib::protocol