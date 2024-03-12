#pragma once

#include <cstring>
#include <string_view>
#include <type_traits>

namespace lib::protocol {
struct message {
  static constexpr const int MAX_MSG_SIZE = 128;
  int msg_size;
  char msg_content[MAX_MSG_SIZE];
  message() { memset(msg_content, 0, MAX_MSG_SIZE); }
  message(const message& other) {
    msg_size = other.msg_size;
    memset(msg_content, 0, sizeof(msg_content));
    memcpy(msg_content, other.msg_content, msg_size);
  }
  message& operator=(const message& other) {
    msg_size = other.msg_size;
    memset(msg_content, 0, sizeof(msg_content));
    memcpy(msg_content, other.msg_content, msg_size);
    return *this;
  }

  template <typename T>
  message(const T& data) : message() {
    if constexpr (std::is_same_v<T, std::string_view>) {
      msg_size = data.size();
      memcpy(msg_content, data.data(), msg_size);
    } else {
      msg_size = sizeof(T);
      memcpy(msg_content, reinterpret_cast<char*>(&data), sizeof(T));
    }
  }
  message(message&& other) {
    msg_size = other.msg_size;
    memset(msg_content, 0, MAX_MSG_SIZE);
    memmove(msg_content, other.msg_content, msg_size);
  }
  message& operator=(message&& other) {
    msg_size = other.msg_size;
    memset(msg_content, 0, MAX_MSG_SIZE);
    memmove(msg_content, other.msg_content, msg_size);
    return *this;
  }
};
}  // namespace lib::protocol