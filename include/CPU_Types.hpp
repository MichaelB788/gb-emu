#pragma once
#include "bit.hpp"
#include <cstdint>

class CPU;

/**
 * Most GameBoy opcodes utilitze the following 8 bit pattern:
 * [xx yyy zzz]
 *
 * x represents which block the opcode belongs in.
 * y and z may represent operands and/or opcode groups.
 */
struct Opcode {
  uint8_t val;

  /// @return The upper two bits of the opcode.
  [[nodiscard]] auto x() const -> uint8_t { return (val >> 6) & 0x3; }

  /// @return The middle three bits of the opcode.
  [[nodiscard]] auto y() const -> uint8_t { return (val >> 3) & 0x7; }

  /// @return The lower three bits of the opcode.
  [[nodiscard]] auto z() const -> uint8_t { return val & 0x7; }
};

struct Instruction {
  Opcode opcode;
  void (CPU::*exec)();
  const char *name;
  uint8_t cycles;
};

/// `union` to represent CPU register pairs for easy 8-bit and 16-bit register
/// access.
union RegisterPair {
  struct {
    uint8_t lo, hi;
  } byte;
  uint16_t word = 0;
};

struct Flags {
  // Bit indicies for the flags Carry, Half-Carry, Subtraction & Zero
  enum Index : uint8_t { C = 4, H = 5, N = 6, Z = 7 };

  uint8_t byte = 0;

  void set(Index flag, bool val) { bit::set(byte, flag, val); }

  [[nodiscard]] auto get(Index flag) const -> bool {
    return bit::get(byte, flag);
  }
};
