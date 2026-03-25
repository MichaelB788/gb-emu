#ifndef GAMEBOY_REGISTERS_HPP
#define GAMEBOY_REGISTERS_HPP
#include <cstdint>

class RegisterPair {
public:
  RegisterPair(uint8_t &_hi, uint8_t &_lo) : hi(_hi), lo(_lo) {}

  [[nodiscard]] uint16_t reg() const {
    return static_cast<uint16_t>(hi << 8) | lo;
  }

  // clang-format off
  void set(uint16_t n16) {
    hi = static_cast<uint8_t>(n16 >> 8); lo = static_cast<uint8_t>(n16 & 0xFF);
  }

  uint16_t inc() { uint16_t ret = reg(); set(reg() + 1); return ret; }

  uint16_t dec() { uint16_t ret = reg(); set(reg() - 1); return ret; }
  // clang-format on

private:
  uint8_t &hi;

  uint8_t &lo;
};

#endif
