#pragma once

#include <cstdint>
#include <ctime>
namespace lib::time {
static uint64_t get_monotonic_usec() noexcept {
  timespec tv = {0, 0};
  clock_gettime(CLOCK_MONOTONIC, &tv);
  return uint64_t(tv.tv_sec) * 1'000'000 + tv.tv_nsec / 1'000;
}

}  // namespace lib::time