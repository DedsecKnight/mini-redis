#pragma once

#include <optional>
#include <string>
#include <unordered_map>

namespace mini_redis::data {
class global_data {
 public:
  std::optional<std::string> get(const std::string& key) const noexcept;
  void set(const std::string& key, const std::string& value) noexcept;
  size_t del(const std::string& key) noexcept { return global_mp_.erase(key); }

 private:
  std::unordered_map<std::string, std::string> global_mp_;
};

}  // namespace mini_redis::data