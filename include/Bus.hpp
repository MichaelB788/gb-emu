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
  [[nodiscard]] uint8_t read_wram_bank_0(const uint16_t addr) const {
    return wram_[addr - WRAM_BANK_0_ADDR];
  }

  void write_wram_bank_0(const uint16_t addr, const uint8_t val) {
    wram_[addr - WRAM_BANK_0_ADDR] = val;
  }

  [[nodiscard]] uint8_t read_echo_ram(const uint16_t addr) const {
    return wram_[addr - ECHO_RAM_ADDR];
  }

  void write_echo_ram(const uint16_t addr, const uint8_t val) {
    wram_[addr - ECHO_RAM_ADDR] = val;
  }

  [[nodiscard]] uint8_t read_hram(const uint16_t addr) const {
    return hram_[addr - HRAM_ADDR];
  }

  void write_hram(const uint16_t addr, const uint8_t val) {
    hram_[addr - HRAM_ADDR] = val;
  }

private:
  Cartridge &cart_;

  PPU ppu_;

  IO io_;

  std::array<uint8_t, KIB_8> wram_{};

  std::array<uint8_t, 127> hram_{};

  uint8_t IE{};
};

#endif // GAMEBOY_BUS_HPP
