#pragma once

#include <cstring>
#include <string_view>
#include <type_traits>

namespace protocol {
struct message {
  static constexpr const int MAX_MSG_SIZE = 4096;
  int msg_size;
  char msg_content[MAX_MSG_SIZE];
  message() { memset(msg_content, 0, MAX_MSG_SIZE); }

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
};
}  // namespace protocol