#include "include/data_structures/sorted_set_container.h"

namespace lib::data_structures {
void sorted_set_container::set_member_data(const std::string& key,
                                           const std::string& member,
                                           double score) noexcept {
  if (kv_in_sorted_set_.find(key) != kv_in_sorted_set_.end() &&
      kv_in_sorted_set_[key].find(member) != kv_in_sorted_set_[key].end()) {
    ss_[key].erase(std::make_pair(kv_in_sorted_set_[key][member], member));
  }
  kv_in_sorted_set_[key][member] = score;
  ss_[key].insert(std::make_pair(score, member));
}
std::optional<double> sorted_set_container::query_member_score(
    const std::string& key, const std::string& member) const noexcept {
  if (ss_.find(key) == ss_.end()) {
    return std::nullopt;
  }
  assert(kv_in_sorted_set_.find(key) != kv_in_sorted_set_.end());
  if (kv_in_sorted_set_.at(key).find(member) ==
      kv_in_sorted_set_.at(key).end()) {
    return std::nullopt;
  }
  return kv_in_sorted_set_.at(key).at(member);
}
void sorted_set_container::erase_member(const std::string& key,
                                        const std::string& member) noexcept {
  if (ss_.find(key) == ss_.end()) {
    return;
  }
  if (kv_in_sorted_set_[key].find(member) == kv_in_sorted_set_[key].end()) {
    return;
  }
  ss_[key].erase(std::make_pair(kv_in_sorted_set_[key][member], member));
  kv_in_sorted_set_[key].erase(member);
}
std::optional<std::vector<std::pair<double, std::string>>>
sorted_set_container::get_key_data(const std::string& key) const noexcept {
  if (ss_.find(key) == ss_.end()) {
    return std::nullopt;
  }
  std::vector<std::pair<double, std::string>> ret;
  for (auto it = ss_.at(key).begin(); it != ss_.at(key).end();
       it = std::next(it)) {
    ret.emplace_back(it->first, it->second);
  }
  return ret;
}
std::vector<std::string> sorted_set_container::find_member_within_score_range(
    const std::string& key, double min_score, double max_score) const noexcept {
  if (ss_.find(key) == ss_.end()) {
    return {};
  }
  if (ss_.at(key).empty()) {
    return {};
  }
  std::vector<std::string> ret;
  auto it = ss_.at(key).lower_bound(std::make_pair(min_score, ""));
  for (; it != ss_.at(key).end() && it->first <= max_score;
       it = std::next(it)) {
    ret.push_back(it->second);
  }
  return ret;
}
bool sorted_set_container::contains_key(const std::string& key) const noexcept {
  return kv_in_sorted_set_.find(key) != kv_in_sorted_set_.end();
}
void sorted_set_container::erase_key(const std::string& key) noexcept {
  ss_.erase(key);
  kv_in_sorted_set_.erase(key);
}
}  // namespace lib::data_structures