#pragma once

#include <memory>
#include <optional>
#include <unordered_map>

#include "include/data_types/ordered_set.h"
namespace lib::data_structures {
class sorted_set_container {
 public:
  bool contains_key(const std::string& key) const noexcept;
  void set_member_data(const std::string& key, const std::string& member,
                       double score) noexcept;
  std::optional<double> query_member_score(
      const std::string& key, const std::string& member) const noexcept;
  void erase_member(const std::string& key, const std::string& member) noexcept;
  std::optional<std::vector<std::pair<double, std::string>>> get_key_data(
      const std::string& key) const noexcept;
  std::vector<std::string> find_member_within_score_range(
      const std::string& key, double min_score,
      double max_score) const noexcept;
  [[nodiscard]] std::pair<
      lib::data_types::ordered_set<std::pair<double, std::string>>*,
      std::unordered_map<std::string, double>*>
  erase_key(const std::string& key) noexcept;

 private:
  std::unordered_map<std::string, std::unique_ptr<lib::data_types::ordered_set<
                                      std::pair<double, std::string>>>>
      ss_;
  std::unordered_map<std::string,
                     std::unique_ptr<std::unordered_map<std::string, double>>>
      kv_in_sorted_set_;
};
}  // namespace lib::data_structures