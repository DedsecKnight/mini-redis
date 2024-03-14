#include "include/data_types/redis_array.h"

#include <cstring>
#include <memory>
#include <numeric>

#include "include/data_types/redis_type.h"
#include "include/data_types/registry.h"

namespace lib::data_types {
std::pair<redis_type::size_type, std::unique_ptr<char>> redis_array::serialize()
    const noexcept {
  int raw_size = std::accumulate(
      data_.begin(), data_.end(),
      static_cast<int>(sizeof(TYPE_CODE) + sizeof(int)),
      [](auto acc, const auto& elem) { return acc + elem->raw_size(); });
  char* ret = new char[raw_size];
  int num_elements = static_cast<int>(data_.size());
  memcpy(ret, &TYPE_CODE, sizeof(TYPE_CODE));
  memcpy(&ret[sizeof(TYPE_CODE)], &num_elements, sizeof(num_elements));
  int offset = sizeof(TYPE_CODE) + sizeof(int);
  for (const auto& elem : data_) {
    auto [elem_raw_size, serialized_elem] = elem->serialize();
    memcpy(&ret[offset], serialized_elem.get(), elem_raw_size);
    offset += elem_raw_size;
  }
  return std::make_pair(raw_size, std::unique_ptr<char>{ret});
}
std::string redis_array::to_string() const noexcept {
  std::string ret{"array { "};
  for (std::size_t i = 0; i < data_.size(); i++) {
    ret += data_[i]->to_string();
    if (i != data_.size() - 1) ret += ", ";
  }
  ret += " }";
  return ret;
}
int redis_array::raw_size() const noexcept {
  return elements_sz_ + sizeof(TYPE_CODE) + sizeof(int);
}
redis_array redis_array::from(char* raw_buffer) noexcept {
  redis_array ret;
  int num_elements;
  memcpy(&num_elements, raw_buffer, sizeof(num_elements));
  int offset = sizeof(num_elements);
  for (int i = 0; i < num_elements; i++) {
    ret.data_.push_back(registry::deserialize(&raw_buffer[offset]));
    offset += ret.data_.back()->raw_size();
    ret.elements_sz_ += ret.data_.back()->raw_size();
  }
  return ret;
}
std::unique_ptr<redis_type> redis_array::clone() const noexcept {
  auto ret = std::make_unique<redis_array>();
  for (const auto& elem : data_) {
    ret->data_.push_back(elem->clone());
    ret->elements_sz_ += elem->raw_size();
  }
  return ret;
}
}  // namespace lib::data_types