#ifndef GAMEBOY_PPU_HPP
#define GAMEBOY_PPU_HPP
#include "Common.hpp"
#include <array>
#include <cstdint>

class PPU {
public:
  PPU() = default;

  [[nodiscard]] uint8_t read_vram(const uint16_t address) const {
    return vram[static_cast<size_t>(address - VRAM_ADDR)];
  }

  // clang-format off
  void write_vram(const uint16_t address, const uint8_t val) { (void)address; (void)val; }
  // clang-format on

private:
  std::array<uint8_t, KIB_8> vram{};
};

#endif
