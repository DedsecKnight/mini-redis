#pragma once

#include <optional>
#include <set>
#include <string>
#include <unordered_map>
#include <utility>

#include "include/data_structures/sorted_set_container.h"

namespace mini_redis::data {
class global_data {
 private:
  static constexpr const int MAX_EVICTION_BATCH_SIZE = 2000;

 public:
  std::optional<std::string> get(const std::string& key) const noexcept;
  void set(const std::string& key, const std::string& value) noexcept;
  size_t del(const std::string& key) noexcept { return global_mp_.erase(key); }
  void set_ttl_ms(const std::string& key, int64_t ttl_ms) noexcept;
  uint64_t get_nearest_ttl_expiration_ts() const noexcept;
  void batch_invalidate_expired_keys() noexcept;
  std::pair<int32_t, uint64_t> get_ttl_ts(
      const std::string& key) const noexcept;
  int sorted_set_set_member_data(const std::string& key,
                                 const std::string& member,
                                 double score) noexcept;
  std::optional<double> sorted_set_query_member_score(
      const std::string& key, const std::string& member) const noexcept;
  void sorted_set_erase_member(const std::string& key,
                               const std::string& member) noexcept;
  std::optional<std::vector<std::pair<double, std::string>>>
  sorted_set_get_key_data(const std::string& key) const noexcept;
  std::vector<std::string> sorted_set_find_member_within_score_range(
      const std::string& key, double min_score,
      double max_score) const noexcept;

 private:
  void invalidate_key(const std::string& key) noexcept;

 private:
  using key_ttl_entry_t = std::pair<uint64_t, std::string>;
  std::unordered_map<std::string, std::string> global_mp_;
  std::unordered_map<std::string, uint64_t> key_to_ttl_;
  std::set<key_ttl_entry_t> ttl_heap_;
  lib::data_structures::sorted_set_container ss_container_;
};

}  // namespace mini_redis::data