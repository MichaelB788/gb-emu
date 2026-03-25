#ifndef GAMEBOY_IO_HPP
#define GAMEBOY_IO_HPP
#include "Common.hpp"
#include <array>
#include <cstdint>

class IO {
public:
  [[nodiscard]] uint8_t read(uint16_t addr) const {
    return io_reg[addr - IO_REG_ADDR];
  }

  void write(uint16_t addr, uint8_t val) { io_reg[addr - IO_REG_ADDR] = val; }

private:
  std::array<uint8_t, 0x80> io_reg{};
};

#endif
