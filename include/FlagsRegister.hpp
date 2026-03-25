#ifndef GAMEBOY_FLAGS_REGISTER_HPP
#define GAMEBOY_FLAGS_REGISTER_HPP
#include "bit.hpp"
#include <cstdint>

class Flags {
public:
  void set_z(bool val) { bit::set(F, Z, val); }

  void set_n(bool val) { bit::set(F, N, val); }

  void set_h(bool val) { bit::set(F, H, val); }

  void set_c(bool val) { bit::set(F, C, val); }

  [[nodiscard]] bool is_z_set() const { return bit::is_set(F, Z); }

  [[nodiscard]] bool is_n_set() const { return bit::is_set(F, N); }

  [[nodiscard]] bool is_h_set() const { return bit::is_set(F, H); }

  [[nodiscard]] bool is_c_set() const { return bit::is_set(F, C); }

  [[nodiscard]] bool is_z_clear() const { return !bit::is_set(F, Z); }

  [[nodiscard]] bool is_c_clear() const { return !bit::is_set(F, C); }

  [[nodiscard]] uint8_t get_carry_bit() const { return bit::get(F, C); }

public:
  void set_raw(uint8_t value) { F = value; }

  [[nodiscard]] uint8_t get_raw() const { return F; }

private:
  static constexpr uint8_t Z = 7, N = 6, H = 5, C = 4;

  uint8_t F{};
};

#endif
