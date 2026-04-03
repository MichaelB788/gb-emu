#include "CPU.hpp"
#include <cassert>
#include <cstdint>
#include <cstdio>

CPU::CPU(Bus &bus) : bus(bus) {}

auto CPU::step() -> uint8_t {
  extra_cycles = 0;

  const auto ins = optable[fetch_byte()];
  opcode = ins.opcode;

  (this->*ins.exec)();

#ifndef NDEBUG
  log_ins(ins);
#endif // !NDEBUG

  return ins.cycles + extra_cycles;
}

auto CPU::read_word(const uint16_t addr) const -> uint16_t {
  const auto lo = bus.read(addr);
  const auto hi = bus.read(addr + 1);
  return static_cast<uint16_t>(hi << 8 | lo);
}

void CPU::write_word(const uint16_t address, const uint16_t value) {
  bus.write(address, static_cast<uint8_t>(value & 0xFF));
  bus.write(address + 1, static_cast<uint8_t>(value >> 8));
}

void CPU::log_ins(const Instruction &ins) const {
  static FILE *output_file = nullptr;
  if (!output_file)
    output_file = fopen("cpu_trace.txt", "w");
  if (!output_file) {
    fprintf(stderr, "Could not open cpu_trace.txt!");
    return;
  }

  fprintf(output_file,
          "[0x%02X]: A:%02X F:%02X BC:%04X DE:%04X HL:%04X "
          "PC:%04X SP:%04X %s\n",
          ins.opcode.val, A, F.byte, BC.word, DE.word, HL.word, PC, SP,
          ins.name);
  fflush(output_file);
}

#if defined(__clang__) || defined(__GNUC__)
#define UNREACHABLE __builtin_unreachable()
#elif defined(_MSC_VER)
#define UNREACHABLE __assume(0)
#else
#define UNREACHABLE assert(false && "This path is unreachable");
#endif

auto CPU::cond(uint8_t y) const -> bool {
  switch (y & 0x3) {
  case 0:
    return !F.get(Flags::Z);
  case 1:
    return F.get(Flags::Z);
  case 2:
    return !F.get(Flags::C);
  case 3:
    return F.get(Flags::C);
  default:
    UNREACHABLE;
  }
}
