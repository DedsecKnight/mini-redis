#include "include/data/global_data.h"

#include <limits>

#include "include/time/manager.h"

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
void global_data::invalidate_key(const std::string& key) noexcept {
  if (key_to_ttl_.find(key) != key_to_ttl_.end()) {
    ttl_heap_.erase(std::make_pair(key_to_ttl_[key], key));
    key_to_ttl_.erase(key);
  }
  global_mp_.erase(key);
}
void global_data::set_ttl_ms(const std::string& key, int64_t ttl_ms) noexcept {
  if (ttl_ms < 0) {
    invalidate_key(key);
  } else {
    uint64_t expired_time = lib::time::get_monotonic_usec() + ttl_ms * 1000;
    if (key_to_ttl_.find(key) != key_to_ttl_.end()) {
      ttl_heap_.erase(std::make_pair(key_to_ttl_[key], key));
    }
    key_to_ttl_[key] = expired_time;
    ttl_heap_.emplace(expired_time, key);
  }
}
uint64_t global_data::get_nearest_ttl_expiration_ts() const noexcept {
  if (!ttl_heap_.empty()) {
    return ttl_heap_.begin()->first;
  }
  return std::numeric_limits<uint64_t>::max();
}
void global_data::batch_invalidate_expired_keys() noexcept {
  uint64_t current_ts = lib::time::get_monotonic_usec() + 1000;
  int num_evicted = 0;
  for (int num_evicted = 0;
       num_evicted <= MAX_EVICTION_BATCH_SIZE && !ttl_heap_.empty() &&
       ttl_heap_.begin()->first <= current_ts;
       num_evicted++) {
    invalidate_key(ttl_heap_.begin()->second);
  }
}
std::pair<int32_t, uint64_t> global_data::get_ttl_ts(
    const std::string& key) const noexcept {
  if (global_mp_.find(key) == global_mp_.end()) {
    return std::make_pair(-2, 0);
  }
  if (key_to_ttl_.find(key) == key_to_ttl_.end()) {
    return std::make_pair(-1, 0);
  }
  return std::make_pair(0, key_to_ttl_.at(key));
}
}  // namespace mini_redis::data