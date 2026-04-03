#include "CPU.hpp"
#include "CPU_Types.hpp"

#if defined(__clang__) || defined(__GNUC__)
#define UNREACHABLE __builtin_unreachable()
#elif defined(_MSC_VER)
#define UNREACHABLE __assume(0)
#else
#define UNREACHABLE assert(false && "This path is unreachable");
#endif

bool is_even(uint8_t num) { return (num & 1) == 0; }

bool is_hl_ind(uint8_t num) { return num == 6; }

auto CPU::block0_ins(Opcode op) -> Instruction {
  switch (op.z()) {
  // JR and misc.
  case 0: {
    switch (op.y()) {
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
    if (is_even(op.y())) {
      return {op, &CPU::ld_r16_n16, "LD r16 n16", 12};
    } else {
      return {op, &CPU::add_hl_r16, "ADD HL r16", 8};
    }
  } break;

  // r16 memory reads and writes
  case 2: {
    switch (op.y()) {
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
    if (is_even(op.y())) {
      return {op, &CPU::inc_r16, "INC r16", 8};
    } else {
      return {op, &CPU::dec_r16, "DEC r16", 8};
    }
  } break;

  // 8-bit register increment
  case 4: {
    if (is_hl_ind(op.y())) {
      return {op, &CPU::inc_mem_hl, "INC (HL)", 12};
    } else {
      return {op, &CPU::inc_r8, "INC r8", 4};
    }
  } break;

  // 8-bit register decrement
  case 5: {
    if (is_hl_ind(op.y())) {
      return {op, &CPU::dec_mem_hl, "DEC (HL)", 12};
    } else {
      return {op, &CPU::dec_r8, "DEC r8", 4};
    }
  } break;

  // 8-bit register immediate loads
  case 6: {
    if (is_hl_ind(op.y())) {
      return {op, &CPU::ld_mem_hl_n8, "LD (HL) n8", 12};
    } else {
      return {op, &CPU::ld_r8_n8, "LD r8 n8", 8};
    }
  } break;

  // Rotations and misc.
  case 7: {
    switch (op.y()) {
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

auto CPU::block1_ins(Opcode op) -> Instruction {
  if (is_hl_ind(op.z()) && is_hl_ind(op.y()))
    return {op, &CPU::halt, "HALT", 4};

  else if (is_hl_ind(op.y()))
    return {op, &CPU::ld_mem_hl_r8, "LD (HL) r8", 8};

  else if (is_hl_ind(op.z()))
    return {op, &CPU::ld_r8_mem_hl, "LD r8 (HL)", 8};

  else
    return {op, &CPU::ld_r8_r8, "LD r8 r8", 4};
}

auto CPU::block2_ins(Opcode op) -> Instruction {
  switch (op.y()) {
  // ADD
  case 0: {
    if (is_hl_ind(op.z())) {
      return {op, &CPU::add_mem_hl, "ADD (HL)", 8};
    } else {
      return {op, &CPU::add_r8, "ADD r8", 4};
    }
  } break;

  // ADC
  case 1: {
    if (is_hl_ind(op.z())) {
      return {op, &CPU::adc_mem_hl, "ADC (HL)", 8};
    } else {
      return {op, &CPU::adc_r8, "ADC r8", 4};
    }
  } break;

  // SUB
  case 2: {
    if (is_hl_ind(op.z())) {
      return {op, &CPU::sub_mem_hl, "SUB (HL)", 8};
    } else {
      return {op, &CPU::sub_r8, "SUB r8", 4};
    }
  } break;

  // SBC
  case 3: {
    if (is_hl_ind(op.z())) {
      return {op, &CPU::sbc_mem_hl, "SBC (HL)", 8};
    } else {
      return {op, &CPU::sbc_r8, "SBC r8", 4};
    }
  } break;

  // AND
  case 4: {
    if (is_hl_ind(op.z())) {
      return {op, &CPU::and_mem_hl, "AND (HL)", 8};
    } else {
      return {op, &CPU::and_r8, "AND r8", 4};
    }
  } break;

  // XOR
  case 5: {
    if (is_hl_ind(op.z())) {
      return {op, &CPU::xor_mem_hl, "XOR (HL)", 8};
    } else {
      return {op, &CPU::xor_r8, "XOR r8", 4};
    }
  } break;

  // OR
  case 6: {
    if (is_hl_ind(op.z())) {
      return {op, &CPU::or_mem_hl, "OR (HL)", 8};
    } else {
      return {op, &CPU::or_r8, "OR r8", 4};
    }
  } break;

  // CP
  case 7: {
    if (is_hl_ind(op.z())) {
      return {op, &CPU::cp_mem_hl, "CP (HL)", 8};
    } else {
      return {op, &CPU::cp_r8, "CP r8", 4};
    }
  } break;
  default:
    UNREACHABLE;
  }
}

auto CPU::block3_ins(Opcode op) -> Instruction {
  switch (op.z()) {
  case 0: {
    switch (op.y()) {
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
    switch (op.y()) {
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
    switch (op.y()) {
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
    switch (op.y()) {
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
    if (op.y() < 4)
      return {op, &CPU::call_cc_a16, "CALL cc a16", 12};
    else
      return {op, &CPU::illegal, "XXX", 0};
  } break;

  case 5: {
    switch (op.y()) {
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
    switch (op.y()) {
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

auto CPU::block0_cb_ins(Opcode op) -> Instruction {
  switch (op.y()) {
  // RLC
  case 0: {
    if (is_hl_ind(op.z())) {
      return {op, &CPU::rlc_mem_hl, "RLC (HL)", 16};
    } else {
      return {op, &CPU::rlc_r8, "RLC r8", 8};
    }
  } break;

  // RRC
  case 1: {
    if (is_hl_ind(op.z())) {
      return {op, &CPU::rrc_mem_hl, "RRC (HL)", 16};
    } else {
      return {op, &CPU::rrc_r8, "RRC r8", 8};
    }
  } break;

  // RL
  case 2: {
    if (is_hl_ind(op.z())) {
      return {op, &CPU::rl_mem_hl, "RL (HL)", 16};
    } else {
      return {op, &CPU::rl_r8, "RL r8", 8};
    }
  } break;

  // RR
  case 3: {
    if (is_hl_ind(op.z())) {
      return {op, &CPU::rr_mem_hl, "RR (HL)", 16};
    } else {
      return {op, &CPU::rr_r8, "RR r8", 8};
    }
  } break;

  // SLA
  case 4: {
    if (is_hl_ind(op.z())) {
      return {op, &CPU::sla_mem_hl, "SLA (HL)", 16};
    } else {
      return {op, &CPU::sla_r8, "SLA r8", 8};
    }
  } break;

  // SRA
  case 5: {
    if (is_hl_ind(op.z())) {
      return {op, &CPU::sra_mem_hl, "SRA (HL)", 16};
    } else {
      return {op, &CPU::sra_r8, "SRA r8", 8};
    }
  } break;

  // SWAP
  case 6: {
    if (is_hl_ind(op.z())) {
      return {op, &CPU::swap_mem_hl, "SWAP (HL)", 16};
    } else {
      return {op, &CPU::swap_r8, "SWAP r8", 8};
    }
  } break;

  // SRL
  case 7: {
    if (is_hl_ind(op.z())) {
      return {op, &CPU::srl_mem_hl, "SRL (HL)", 16};
    } else {
      return {op, &CPU::srl_r8, "SRL r8", 8};
    }
  } break;

  default:
    UNREACHABLE;
  }
}

auto CPU::block1_cb_ins(Opcode op) -> Instruction {
  if (is_hl_ind(op.z())) {
    return {op, &CPU::bit_mem_hl, "BIT b3 (HL)", 12};
  } else {
    return {op, &CPU::bit_r8, "BIT b3 r8", 8};
  }
}

auto CPU::block2_cb_ins(Opcode op) -> Instruction {
  if (is_hl_ind(op.z())) {
    return {op, &CPU::res_mem_hl, "RES b3 (HL)", 16};
  } else {
    return {op, &CPU::res_r8, "RES b3 r8", 8};
  }
}

auto CPU::block3_cb_ins(Opcode op) -> Instruction {
  if (is_hl_ind(op.z())) {
    return {op, &CPU::set_mem_hl, "SET b3 (HL)", 16};
  } else {
    return {op, &CPU::set_r8, "SET b3 r8", 8};
  }
}
