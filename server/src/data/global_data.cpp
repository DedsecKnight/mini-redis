#include "include/data/global_data.h"

namespace mini_redis::data {
std::optional<std::string> global_data::get(
    const std::string& key) const noexcept {
  auto it = global_mp_.find(key);
  if (it == global_mp_.end()) {
    return std::nullopt;
  }
  return it->second;
}
void global_data::set(const std::string& key,
                      const std::string& value) noexcept {
  global_mp_.emplace(key, value);
}
}  // namespace mini_redis::data