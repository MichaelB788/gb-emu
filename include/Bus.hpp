#ifndef GAMEBOY_BUS_HPP
#define GAMEBOY_BUS_HPP
#include "Common.hpp"
#include "IO.hpp"
#include "PPU.hpp"
#include <array>
#include <cstdint>

class Cartridge;

class Bus {
public:
  explicit Bus(Cartridge &cart) : cart_(cart) {}

  [[nodiscard]] uint8_t read(uint16_t addr) const;

  void write(uint16_t addr, uint8_t val);

private:
  Cartridge &cart_;

  PPU ppu_;

  IO io_;

  std::array<uint8_t, KIB_8> wram_{};

  std::array<uint8_t, 127> hram_{};

  uint8_t IE{};
};

#endif // GAMEBOY_BUS_HPP
