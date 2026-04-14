#include "CPU.hpp"
#include <cassert>
#include <cstdint>
#include <cstdio>

CPU::CPU(Bus &bus) : bus(bus) {}

auto CPU::step() -> uint8_t {
  extra_cycles = 0;

  IR = Opcode(fetch_byte());
  const auto ins = optable[IR.val];

  (this->*ins.exec)();

#ifndef NDEBUG
  log_ins(ins);
#endif // !NDEBUG

  return ins.cycles + extra_cycles;
}

void CPU::log_ins(const Instruction &ins) {
  static FILE *output_file = nullptr;
  if (!output_file)
    output_file = fopen("cpu_trace.txt", "w");
  if (!output_file) {
    fprintf(stderr, "Could not open cpu_trace.txt!");
    return;
  }

  static constexpr int MAX_ENTRIES = 10;
  if (num_entries < MAX_ENTRIES) {
    fprintf(output_file,
            "[0x%02X]: A:%02X F:%02X BC:%04X DE:%04X HL:%04X "
            "PC:%04X SP:%04X n16:%04X %s\n",
            IR.val, A, F.byte, BC.word, DE.word, HL.word, PC, SP, read_word(PC),
            ins.name);
    fflush(output_file);
    ++num_entries;
  }
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
