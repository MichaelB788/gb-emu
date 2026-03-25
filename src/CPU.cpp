#include "CPU.hpp"
#include "bit.hpp"
#include <cassert>
#include <cstdint>
#include <cstdio>
#include <iostream>

#if defined(__clang__) || defined(__GNUC__)
#define UNREACHABLE __builtin_unreachable()
#elif defined(_MSC_VER)
#define UNREACHABLE __assume(0)
#else
#define UNREACHABLE assert(false && "This path is unreachable");
#endif

CPU::CPU(Bus &bus) : bus(bus) {}

uint8_t CPU::step() {
  extra_cycles = 0;
  Instruction ins = optable[fetch_byte()];
  opcode = ins.op;

  (this->*ins.exec)();

#ifndef NDEBUG
  log_ins(ins);
#endif // !NDEBUG

  return ins.cycles + extra_cycles;
}

void CPU::write_word(const uint16_t address, const uint16_t value) {
  bus.write(address, static_cast<uint8_t>(value & 0xFF));
  bus.write(address + 1, static_cast<uint8_t>(value >> 8));
}

uint16_t CPU::fetch_word() {
  const uint8_t lo = bus.read(PC++);
  const uint8_t hi = bus.read(PC++);
  return static_cast<uint16_t>(hi << 8 | lo);
}

bool CPU::cond(uint8_t mid) const {
  switch (mid & 0x3) {
  case 0:
    return !is_flag_set(Flag::Z);
  case 1:
    return is_flag_set(Flag::Z);
  case 2:
    return !is_flag_set(Flag::C);
  case 3:
    return is_flag_set(Flag::C);
  default:
    UNREACHABLE;
  }
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
          ins.op.val, A, F, BC.word, DE.word, HL.word, PC, SP, ins.name);
  fflush(output_file);
}

uint8_t CPU::impl_adc_n8(const uint8_t n8) {
  const uint16_t sum = A + n8 + get_flag(Flag::C);
  const auto result = static_cast<uint8_t>(sum);

  set_flag(Flag::Z, result == 0);
  set_flag(Flag::N, false);
  set_flag(Flag::H, (A & 0xF) + (n8 & 0xF) + get_flag(Flag::C) > 0xF);
  set_flag(Flag::C, sum > 0xFF);

  return result;
}

uint8_t CPU::impl_add_n8(const uint8_t n8) {
  const uint16_t sum = A + n8;
  const auto result = static_cast<uint8_t>(sum);

  set_flag(Flag::Z, result == 0);
  set_flag(Flag::N, false);
  set_flag(Flag::H, (A & 0xF) + (n8 & 0xF) > 0xF);
  set_flag(Flag::C, sum > 0xFF);

  return result;
}

uint16_t CPU::impl_add_n16(const uint16_t n16) {
  const uint32_t sum = HL.word + n16;

  set_flag(Flag::N, false);
  set_flag(Flag::H, (HL.word & 0xFFF) + (n16 & 0xFFF) > 0xFFF);
  set_flag(Flag::C, sum > 0xFFFF);

  return static_cast<uint16_t>(sum);
}

uint16_t CPU::impl_add_sp_e8() {
  const uint8_t imm8 = fetch_byte();
  const uint16_t result = SP + static_cast<int8_t>(imm8);

  set_flag(Flag::Z, false);
  set_flag(Flag::N, false);
  set_flag(Flag::H, (SP & 0xF) + (imm8 & 0xF) > 0xF);
  set_flag(Flag::C, (SP & 0xFF) + imm8 > 0xFF);

  return result;
}

uint8_t CPU::impl_dec_n8(const uint8_t n8) {
  const uint8_t result = n8 - 1;

  set_flag(Flag::Z, result == 0);
  set_flag(Flag::N, true);
  set_flag(Flag::H, (n8 & 0xF) == 0);

  return result;
}

uint8_t CPU::impl_inc_n8(const uint8_t n8) {
  const uint8_t result = n8 + 1;

  set_flag(Flag::Z, result == 0);
  set_flag(Flag::N, false);
  set_flag(Flag::H, (n8 & 0xF) + 1 > 0xF);

  return result;
}

uint8_t CPU::impl_sbc_n8(const uint8_t n8) {
  const uint8_t carry = get_flag(Flag::C);
  const uint8_t result = A - (n8 + carry);

  set_flag(Flag::Z, result == 0);
  set_flag(Flag::N, true);
  set_flag(Flag::H, (n8 & 0xF) + carry > (A & 0xF));
  set_flag(Flag::C, n8 + carry > A);

  return result;
}

uint8_t CPU::impl_sub_n8(const uint8_t n8) {
  const uint8_t result = A - n8;

  set_flag(Flag::Z, result == 0);
  set_flag(Flag::N, true);
  set_flag(Flag::H, (n8 & 0xF) > (A & 0xF));
  set_flag(Flag::C, n8 > A);

  return result;
}

uint8_t CPU::impl_and_n8(const uint8_t n8) {
  const uint8_t result = A & n8;

  set_flag(Flag::Z, result == 0);
  set_flag(Flag::N, false);
  set_flag(Flag::H, true);
  set_flag(Flag::C, false);

  return result;
}

uint8_t CPU::impl_or_n8(const uint8_t n8) {
  const uint8_t result = A | n8;

  set_flag(Flag::Z, result == 0);
  set_flag(Flag::N, false);
  set_flag(Flag::H, false);
  set_flag(Flag::C, false);

  return result;
}

uint8_t CPU::impl_xor_n8(const uint8_t n8) {
  const uint8_t result = A ^ n8;

  set_flag(Flag::Z, result == 0);
  set_flag(Flag::N, false);
  set_flag(Flag::H, false);
  set_flag(Flag::C, false);

  return result;
}

uint8_t CPU::impl_rl_n8(const uint8_t n8, const Prefix prefix) {
  const uint8_t result = (n8 << 1) | get_flag(Flag::C);

  set_flag(Flag::Z, result == 0 && prefix == Prefix::CB);
  set_flag(Flag::N, false);
  set_flag(Flag::H, false);
  set_flag(Flag::C, bit::is_set(n8, 7));

  return result;
}

uint8_t CPU::impl_rlc_n8(const uint8_t n8, const Prefix prefix) {
  const uint8_t result = n8 << 1 | bit::get(n8, 7);

  set_flag(Flag::Z, result == 0 && prefix == Prefix::CB);
  set_flag(Flag::N, false);
  set_flag(Flag::H, false);
  set_flag(Flag::C, bit::is_set(n8, 7));

  return result;
}

uint8_t CPU::impl_rr_n8(const uint8_t n8, const Prefix prefix) {
  const uint8_t result = n8 >> 1 | get_flag(Flag::C) << 7;

  set_flag(Flag::Z, result == 0 && prefix == Prefix::CB);
  set_flag(Flag::N, false);
  set_flag(Flag::H, false);
  set_flag(Flag::C, bit::is_set(n8, 0));

  return result;
}

uint8_t CPU::impl_rrc_n8(const uint8_t n8, const Prefix prefix) {
  const uint8_t result = n8 >> 1 | (bit::get(n8, 0)) << 7;

  set_flag(Flag::Z, result == 0 && prefix == Prefix::CB);
  set_flag(Flag::N, false);
  set_flag(Flag::H, false);
  set_flag(Flag::C, bit::is_set(n8, 0));

  return result;
}

uint8_t CPU::impl_sla_n8(const uint8_t n8) {
  const uint8_t result = n8 << 1;

  set_flag(Flag::Z, result == 0);
  set_flag(Flag::N, false);
  set_flag(Flag::H, false);
  set_flag(Flag::C, bit::is_set(n8, 7));

  return result;
}

uint8_t CPU::impl_sra_n8(const uint8_t n8) {
  const uint8_t result = n8 >> 1 | (bit::get(n8, 7) << 7);

  set_flag(Flag::Z, result == 0);
  set_flag(Flag::N, false);
  set_flag(Flag::H, false);
  set_flag(Flag::C, bit::is_set(n8, 0));

  return result;
}

uint8_t CPU::impl_srl_n8(const uint8_t n8) {
  const uint8_t result = n8 >> 1;

  set_flag(Flag::Z, result == 0);
  set_flag(Flag::N, false);
  set_flag(Flag::H, false);
  set_flag(Flag::C, bit::is_set(n8, 0));

  return result;
}

uint8_t CPU::impl_swap_n8(const uint8_t n8) {
  const uint8_t hi = (n8 & 0xF) << 4;
  const uint8_t lo = (n8 & 0xF0) >> 4;
  const uint8_t result = hi | lo;

  set_flag(Flag::Z, result == 0);
  set_flag(Flag::N, false);
  set_flag(Flag::H, false);
  set_flag(Flag::C, false);

  return result;
}

void CPU::res_mem_hl() {
  uint8_t hl_ind = read_hl();
  bit::set(hl_ind, opcode.mid, false);
  write_hl(hl_ind);
}

void CPU::set_mem_hl() {
  uint8_t hl_ind = read_hl();
  bit::set(hl_ind, opcode.mid, true);
  write_hl(hl_ind);
}

void CPU::call_cc_a16() {
  uint16_t addr = fetch_word();
  if (cond(opcode.mid)) {
    SP -= 2;
    write_word(SP, PC);
    PC = addr;
    extra_cycles = 12;
  }
}

void CPU::daa() {
  uint8_t result = 0;
  uint8_t adjustment = 0;

  if (is_flag_set(Flag::N)) {
    if (is_flag_set(Flag::H)) {
      adjustment += 0x6;
    }

    if (is_flag_set(Flag::C)) {
      adjustment += 0x60;
    }

    result = A - adjustment;
  } else {
    if (is_flag_set(Flag::H) || (A & 0xF) > 0x9) {
      adjustment += 0x6;
    }

    if (is_flag_set(Flag::C) || A > 0x99) {
      adjustment += 0x60;
      set_flag(Flag::C, true);
    }

    result = A + adjustment;
  }

  set_flag(Flag::Z, result == 0);
  set_flag(Flag::H, false);

  A = result;
}

void CPU::prefix() {
  Instruction cb_ins = cb_optable[fetch_byte()];
  opcode = cb_ins.op;

  (this->*cb_ins.exec)();

#ifndef NDEBUG
  log_ins(cb_ins);
#endif
}

void CPU::illegal() {
  std::cerr << "Illegal opcode encountered: 0x" << std::hex
            << static_cast<int>(opcode.val) << "\n";
  state = State::Stop;
}

bool is_even(uint8_t num) { return (num & 1) == 0; }

bool is_hl_ind(uint8_t num) { return num == 6; }

CPU::Instruction CPU::block0_ins(Opcode op) {
  switch (op.lo) {
  // JR and misc.
  case 0: {
    switch (op.mid) {
    case 0:
      return {op, &CPU::nop, "NOP", 4};
    case 1:
      return {op, &CPU::ld_a16_sp, "LD (a16) SP", 4};
    case 2:
      return {op, &CPU::stop, "STOP n8", 4};
    case 3:
      return {op, &CPU::jr_e8, "JR e8", 12};
    case 4:
    case 5:
    case 6:
    case 7:
      return {op, &CPU::jr_cc_e8, "JR cc e8", 8};
    default:
      UNREACHABLE;
    }
  } break;

  // r16 loads and arithmetic
  case 1: {
    if (is_even(op.mid)) {
      return {op, &CPU::ld_r16_n16, "LD r16 n16", 12};
    } else {
      return {op, &CPU::add_hl_r16, "ADD HL r16", 8};
    }
  } break;

  // r16 memory reads and writes
  case 2: {
    switch (op.mid) {
    case 0:
    case 2:
      return {op, &CPU::ld_mem_r16_a, "LD (r16) A", 8};
      break;
    case 1:
    case 3:
      return {op, &CPU::ld_a_mem_r16, "LD A (r16)", 8};
      break;
    case 4:
      return {op, &CPU::ld_mem_hli_a, "LD (HLI) A", 8};
      break;
    case 5:
      return {op, &CPU::ld_mem_hld_a, "LD (HLD) A", 8};
      break;
    case 6:
      return {op, &CPU::ld_a_mem_hli, "LD A (HLI)", 8};
      break;
    case 7:
      return {op, &CPU::ld_a_mem_hld, "LD A (HLD)", 8};
      break;
    default:
      UNREACHABLE;
    }
  } break;

  // 16-bit register increment and decrement
  case 3: {
    if (is_even(op.mid)) {
      return {op, &CPU::inc_r16, "INC r16", 8};
    } else {
      return {op, &CPU::dec_r16, "DEC r16", 8};
    }
  } break;

  // 8-bit register increment
  case 4: {
    if (is_hl_ind(op.mid)) {
      return {op, &CPU::inc_mem_hl, "INC (HL)", 12};
    } else {
      return {op, &CPU::inc_r8, "INC r8", 4};
    }
  } break;

  // 8-bit register decrement
  case 5: {
    if (is_hl_ind(op.mid)) {
      return {op, &CPU::dec_mem_hl, "DEC (HL)", 12};
    } else {
      return {op, &CPU::dec_r8, "DEC r8", 4};
    }
  } break;

  // 8-bit register immediate loads
  case 6: {
    if (is_hl_ind(op.mid)) {
      return {op, &CPU::ld_mem_hl_n8, "LD (HL) n8", 12};
    } else {
      return {op, &CPU::ld_r8_n8, "LD r8 n8", 8};
    }
  } break;

  // Rotations and misc.
  case 7: {
    switch (op.mid) {
    case 0:
      return {op, &CPU::rlca, "RLCA", 4};
      break;
    case 1:
      return {op, &CPU::rrca, "RRCA", 4};
      break;
      break;
    case 2:
      return {op, &CPU::rla, "RLA", 4};
      break;
    case 3:
      return {op, &CPU::rra, "RRA", 4};
      break;
    case 4:
      return {op, &CPU::daa, "DAA", 4};
      break;
    case 5:
      return {op, &CPU::cpl, "CPL", 4};
      break;
    case 6:
      return {op, &CPU::scf, "SCF", 4};
      break;
    case 7:
      return {op, &CPU::ccf, "CCF", 4};
      break;
    default:
      UNREACHABLE;
    }
  } break;
  default:
    UNREACHABLE;
  }
}

CPU::Instruction CPU::block1_ins(Opcode op) {
  if (is_hl_ind(op.mid) && is_hl_ind(op.lo))
    return {op, &CPU::halt, "HALT", 4};

  else if (is_hl_ind(op.mid))
    return {op, &CPU::ld_mem_hl_r8, "LD (HL) r8", 8};

  else if (is_hl_ind(op.lo))
    return {op, &CPU::ld_r8_mem_hl, "LD r8 (HL)", 8};

  else
    return {op, &CPU::ld_r8_r8, "LD r8 r8", 4};
}

CPU::Instruction CPU::block2_ins(Opcode op) {
  switch (op.mid) {
  // ADD
  case 0: {
    if (is_hl_ind(op.lo)) {
      return {op, &CPU::add_mem_hl, "ADD (HL)", 8};
    } else {
      return {op, &CPU::add_r8, "ADD r8", 4};
    }
  } break;

  // ADC
  case 1: {
    if (is_hl_ind(op.lo)) {
      return {op, &CPU::adc_mem_hl, "ADC (HL)", 8};
    } else {
      return {op, &CPU::adc_r8, "ADC r8", 4};
    }
  } break;

  // SUB
  case 2: {
    if (is_hl_ind(op.lo)) {
      return {op, &CPU::sub_mem_hl, "SUB (HL)", 8};
    } else {
      return {op, &CPU::sub_r8, "SUB r8", 4};
    }
  } break;

  // SBC
  case 3: {
    if (is_hl_ind(op.lo)) {
      return {op, &CPU::sbc_mem_hl, "SBC (HL)", 8};
    } else {
      return {op, &CPU::sbc_r8, "SBC r8", 4};
    }
  } break;

  // AND
  case 4: {
    if (is_hl_ind(op.lo)) {
      return {op, &CPU::and_mem_hl, "AND (HL)", 8};
    } else {
      return {op, &CPU::and_r8, "AND r8", 4};
    }
  } break;

  // XOR
  case 5: {
    if (is_hl_ind(op.lo)) {
      return {op, &CPU::xor_mem_hl, "XOR (HL)", 8};
    } else {
      return {op, &CPU::xor_r8, "XOR r8", 4};
    }
  } break;

  // OR
  case 6: {
    if (is_hl_ind(op.lo)) {
      return {op, &CPU::or_mem_hl, "OR (HL)", 8};
    } else {
      return {op, &CPU::or_r8, "OR r8", 4};
    }
  } break;

  // CP
  case 7: {
    if (is_hl_ind(op.lo)) {
      return {op, &CPU::cp_mem_hl, "CP (HL)", 8};
    } else {
      return {op, &CPU::cp_r8, "CP r8", 4};
    }
  } break;
  default:
    UNREACHABLE;
  }
}

CPU::Instruction CPU::block3_ins(Opcode op) {
  switch (op.lo) {
  case 0: {
    switch (op.mid) {
    case 0:
    case 1:
    case 2:
    case 3:
      return {op, &CPU::ret_cc, "RET cc", 8};
    case 4:
      return {op, &CPU::ldh_a8_a, "LDH (a8) A", 12};
    case 5:
      return {op, &CPU::add_sp_e8, "ADD SP e8", 16};
    case 6:
      return {op, &CPU::ldh_a_a8, "LDH A (a8)", 12};
    case 7:
      return {op, &CPU::ld_hl_sp_e8, "LD HL SP+e8", 12};
    default:
      UNREACHABLE;
    }
  } break;

  case 1: {
    switch (op.mid) {
    case 0:
    case 2:
    case 4:
      return {op, &CPU::pop_r16, "POP r16", 12};
    case 6:
      return {op, &CPU::pop_af, "POP AF", 12};
    case 1:
      return {op, &CPU::ret, "RET", 16};
    case 3:
      return {op, &CPU::reti, "RETI", 16};
    case 5:
      return {op, &CPU::jp_hl, "JP HL", 4};
    case 7:
      return {op, &CPU::ld_sp_hl, "LD SP HL", 8};
    default:
      UNREACHABLE;
    }
  } break;

  case 2: {
    switch (op.mid) {
    case 0:
    case 1:
    case 2:
    case 3:
      return {op, &CPU::jp_cc_a16, "JP cc n16", 12};
    case 4:
      return {op, &CPU::ldh_mem_c_a, "LDH (C) A", 8};
    case 5:
      return {op, &CPU::ld_a16_a, "LD (a16) A", 16};
    case 6:
      return {op, &CPU::ldh_a_mem_c, "LDH A (C)", 8};
    case 7:
      return {op, &CPU::ld_a_a16, "LD A (a16)", 16};
    default:
      UNREACHABLE;
    }
  } break;

  case 3: {
    switch (op.mid) {
    case 0:
      return {op, &CPU::jp_a16, "JP a16", 16};
    case 1:
      return {op, &CPU::prefix, "PREFIX", 4};
    case 2:
    case 3:
    case 4:
    case 5:
      return {op, &CPU::illegal, "XXX", 0};
    case 6:
      return {op, &CPU::di, "DI", 4};
    case 7:
      return {op, &CPU::ei, "EI", 4};
    default:
      UNREACHABLE;
    }
  } break;

  case 4: {
    if (op.mid < 4)
      return {op, &CPU::call_cc_a16, "CALL cc a16", 12};
    else
      return {op, &CPU::illegal, "XXX", 0};
  } break;

  case 5: {
    switch (op.mid) {
    case 0:
    case 2:
    case 4:
      return {op, &CPU::push_r16, "PUSH r16", 16};
    case 6:
      return {op, &CPU::push_af, "PUSH AF", 16};
    case 1:
      return {op, &CPU::call_a16, "CALL a16", 24};
    case 3:
    case 5:
    case 7:
      return {op, &CPU::illegal, "XXX", 0};
    default:
      UNREACHABLE;
    }
  } break;

  case 6: {
    switch (op.mid) {
    case 0:
      return {op, &CPU::add_n8, "ADD n8", 8};
    case 1:
      return {op, &CPU::adc_imm8, "ADC n8", 8};
    case 2:
      return {op, &CPU::sub_imm8, "SUB n8", 8};
    case 3:
      return {op, &CPU::sbc_imm8, "SBC n8", 8};
    case 4:
      return {op, &CPU::and_imm8, "AND n8", 8};
    case 5:
      return {op, &CPU::xor_imm8, "XOR n8", 8};
    case 6:
      return {op, &CPU::or_imm8, "OR n8", 8};
    case 7:
      return {op, &CPU::cp_imm8, "CP n8", 8};
    default:
      UNREACHABLE;
    }
  } break;

  case 7: {
    return {op, &CPU::rst_vec, "RST", 16};
  } break;

  default:
    UNREACHABLE;
  }
}

CPU::Instruction CPU::block0_cb_ins(Opcode op) {
  switch (op.mid) {
  // RLC
  case 0: {
    if (is_hl_ind(op.lo)) {
      return {op, &CPU::rlc_mem_hl, "RLC (HL)", 16};
    } else {
      return {op, &CPU::rlc_r8, "RLC r8", 8};
    }
  } break;

  // RRC
  case 1: {
    if (is_hl_ind(op.lo)) {
      return {op, &CPU::rrc_mem_hl, "RRC (HL)", 16};
    } else {
      return {op, &CPU::rrc_r8, "RRC r8", 8};
    }
  } break;

  // RL
  case 2: {
    if (is_hl_ind(op.lo)) {
      return {op, &CPU::rl_mem_hl, "RL (HL)", 16};
    } else {
      return {op, &CPU::rl_r8, "RL r8", 8};
    }
  } break;

  // RR
  case 3: {
    if (is_hl_ind(op.lo)) {
      return {op, &CPU::rr_mem_hl, "RR (HL)", 16};
    } else {
      return {op, &CPU::rr_r8, "RR r8", 8};
    }
  } break;

  // SLA
  case 4: {
    if (is_hl_ind(op.lo)) {
      return {op, &CPU::sla_mem_hl, "SLA (HL)", 16};
    } else {
      return {op, &CPU::sla_r8, "SLA r8", 8};
    }
  } break;

  // SRA
  case 5: {
    if (is_hl_ind(op.lo)) {
      return {op, &CPU::sra_mem_hl, "SRA (HL)", 16};
    } else {
      return {op, &CPU::sra_r8, "SRA r8", 8};
    }
  } break;

  // SWAP
  case 6: {
    if (is_hl_ind(op.lo)) {
      return {op, &CPU::swap_mem_hl, "SWAP (HL)", 16};
    } else {
      return {op, &CPU::swap_r8, "SWAP r8", 8};
    }
  } break;

  // SRL
  case 7: {
    if (is_hl_ind(op.lo)) {
      return {op, &CPU::srl_mem_hl, "SRL (HL)", 16};
    } else {
      return {op, &CPU::srl_r8, "SRL r8", 8};
    }
  } break;

  default:
    UNREACHABLE;
  }
}

CPU::Instruction CPU::block1_cb_ins(Opcode op) {
  if (is_hl_ind(op.lo)) {
    return {op, &CPU::bit_mem_hl, "BIT b3 (HL)", 12};
  } else {
    return {op, &CPU::bit_r8, "BIT b3 r8", 8};
  }
}

CPU::Instruction CPU::block2_cb_ins(Opcode op) {
  if (is_hl_ind(op.lo)) {
    return {op, &CPU::res_mem_hl, "RES b3 (HL)", 16};
  } else {
    return {op, &CPU::res_r8, "RES b3 r8", 8};
  }
}

CPU::Instruction CPU::block3_cb_ins(Opcode op) {
  if (is_hl_ind(op.lo)) {
    return {op, &CPU::set_mem_hl, "SET b3 (HL)", 16};
  } else {
    return {op, &CPU::set_r8, "SET b3 r8", 8};
  }
}
