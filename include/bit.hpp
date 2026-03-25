#ifndef BIT_HPP
#define BIT_HPP
#include <cassert>
#include <cstdint>

namespace bit {
inline void set(uint8_t &u8, uint8_t idx, bool val) {
  assert(idx < 8 && "Cannot set the bit at this index");
  val ? u8 |= static_cast<uint8_t>(1 << idx)
      : u8 &= ~static_cast<uint8_t>(1 << idx);
}

[[nodiscard]] inline uint8_t get(uint8_t u8, uint8_t idx) {
  assert(idx < 8 && "Cannot get the bit at this index");
  return (u8 >> idx) & 1;
}

[[nodiscard]] inline bool is_set(uint8_t u8, uint8_t idx) {
  assert(idx < 8 && "Cannot check if the bit at this index is set");
  return static_cast<bool>(get(u8, idx));
}
} // namespace bit

#endif
