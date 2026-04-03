#pragma once
#include <cassert>
#include <cstdint>

namespace bit {
inline void set(uint8_t &u8, uint8_t idx, bool val) {
  assert(idx < 8 && "Cannot set the bit at this index");
  if (val) {
    u8 |= static_cast<uint8_t>(1 << idx);
  } else {
    u8 &= ~static_cast<uint8_t>(1 << idx);
  }
}

[[nodiscard]] inline auto get(uint8_t u8, uint8_t idx) -> bool {
  assert(idx < 8 && "Cannot get the bit at this index");
  return (u8 >> idx) & 1;
}
} // namespace bit
