#include "CPU.hpp"
#include "CPU_Types.hpp"

#if defined(__clang__) || defined(__GNUC__)
#define UNREACHABLE __builtin_unreachable()
#elif defined(_MSC_VER)
#define UNREACHABLE __assume(0)
#else
#define UNREACHABLE assert(false && "This path is unreachable");
#endif

namespace {
bool is_even(uint8_t num) { return (num & 1) == 0; }

bool is_hl_ind(uint8_t num) { return num == 6; }
} // namespace

auto CPU::block0_ins(Opcode op) -> Instruction {
  switch (op.z()) {
  // JR and misc.
  case 0:
    switch (op.y()) {
    case 0:
      return {"NOP", 4, &CPU::nop};
    case 1:
      return {"LD (a16) SP", 4, &CPU::ld_a16_sp};
    case 2:
      return {"STOP n8", 4, &CPU::stop};
    case 3:
      return {"JR e8", 12, &CPU::jr_e8};
    case 4:
    case 5:
    case 6:
    case 7:
      return {"JR cc e8", 8, &CPU::jr_cc_e8};
    default:
      UNREACHABLE;
    }

  // r16 loads and arithmetic
  case 1:
    if (is_even(op.y())) {
      return {"LD r16 n16", 12, &CPU::ld_r16_n16};
    } else {
      return {"ADD HL r16", 8, &CPU::add_hl_r16};
    }

  // r16 memory reads and writes
  case 2:
    switch (op.y()) {
    case 0:
    case 2:
      return {"LD (r16) A", 8, &CPU::ld_mem_r16_a};
    case 1:
    case 3:
      return {"LD A (r16)", 8, &CPU::ld_a_mem_r16};
    case 4:
      return {"LD (HLI) A", 8, &CPU::ld_mem_hli_a};
    case 5:
      return {"LD (HLD) A", 8, &CPU::ld_mem_hld_a};
    case 6:
      return {"LD A (HLI)", 8, &CPU::ld_a_mem_hli};
    case 7:
      return {"LD A (HLD)", 8, &CPU::ld_a_mem_hld};
    default:
      UNREACHABLE;
    }

  // 16-bit register increment and decrement
  case 3:
    if (is_even(op.y())) {
      return {"INC r16", 8, &CPU::inc_r16};
    } else {
      return {"DEC r16", 8, &CPU::dec_r16};
    }

  // 8-bit register increment
  case 4:
    if (is_hl_ind(op.y())) {
      return {"INC (HL)", 12, &CPU::inc_mem_hl};
    } else {
      return {"INC r8", 4, &CPU::inc_r8};
    }

  // 8-bit register decrement
  case 5:
    if (is_hl_ind(op.y())) {
      return {"DEC (HL)", 12, &CPU::dec_mem_hl};
    } else {
      return {"DEC r8", 4, &CPU::dec_r8};
    }

  // 8-bit register immediate loads
  case 6:
    if (is_hl_ind(op.y())) {
      return {"LD (HL) n8", 12, &CPU::ld_mem_hl_n8};
    } else {
      return {"LD r8 n8", 8, &CPU::ld_r8_n8};
    }

  // Rotations and misc.
  case 7:
    switch (op.y()) {
    case 0:
      return {"RLCA", 4, &CPU::rlca};
    case 1:
      return {"RRCA", 4, &CPU::rrca};
    case 2:
      return {"RLA", 4, &CPU::rla};
    case 3:
      return {"RRA", 4, &CPU::rra};
    case 4:
      return {"DAA", 4, &CPU::daa};
    case 5:
      return {"CPL", 4, &CPU::cpl};
    case 6:
      return {"SCF", 4, &CPU::scf};
    case 7:
      return {"CCF", 4, &CPU::ccf};
    default:
      UNREACHABLE;
    }

  default:
    UNREACHABLE;
  }
}

auto CPU::block1_ins(Opcode op) -> Instruction {
  if (is_hl_ind(op.z()) && is_hl_ind(op.y()))
    return {"HALT", 4, &CPU::halt};
  else if (is_hl_ind(op.y()))
    return {"LD (HL) r8", 8, &CPU::ld_mem_hl_r8};
  else if (is_hl_ind(op.z()))
    return {"LD r8 (HL)", 8, &CPU::ld_r8_mem_hl};
  else
    return {"LD r8 r8", 4, &CPU::ld_r8_r8};
}

auto CPU::block2_ins(Opcode op) -> Instruction {
  switch (op.y()) {
  // ADD
  case 0:
    if (is_hl_ind(op.z())) {
      return {"ADD (HL)", 8, &CPU::add_mem_hl};
    } else {
      return {"ADD r8", 4, &CPU::add_r8};
    }

  // ADC
  case 1:
    if (is_hl_ind(op.z())) {
      return {"ADC (HL)", 8, &CPU::adc_mem_hl};
    } else {
      return {"ADC r8", 4, &CPU::adc_r8};
    }

  // SUB
  case 2:
    if (is_hl_ind(op.z())) {
      return {"SUB (HL)", 8, &CPU::sub_mem_hl};
    } else {
      return {"SUB r8", 4, &CPU::sub_r8};
    }

  // SBC
  case 3:
    if (is_hl_ind(op.z())) {
      return {"SBC (HL)", 8, &CPU::sbc_mem_hl};
    } else {
      return {"SBC r8", 4, &CPU::sbc_r8};
    }

  // AND
  case 4:
    if (is_hl_ind(op.z())) {
      return {"AND (HL)", 8, &CPU::and_mem_hl};
    } else {
      return {"AND r8", 4, &CPU::and_r8};
    }

  // XOR
  case 5:
    if (is_hl_ind(op.z())) {
      return {"XOR (HL)", 8, &CPU::xor_mem_hl};
    } else {
      return {"XOR r8", 4, &CPU::xor_r8};
    }

  // OR
  case 6:
    if (is_hl_ind(op.z())) {
      return {"OR (HL)", 8, &CPU::or_mem_hl};
    } else {
      return {"OR r8", 4, &CPU::or_r8};
    }

  // CP
  case 7:
    if (is_hl_ind(op.z())) {
      return {"CP (HL)", 8, &CPU::cp_mem_hl};
    } else {
      return {"CP r8", 4, &CPU::cp_r8};
    }

  default:
    UNREACHABLE;
  }
}

auto CPU::block3_ins(Opcode op) -> Instruction {
  switch (op.z()) {
  case 0:
    switch (op.y()) {
    case 0:
    case 1:
    case 2:
    case 3:
      return {"RET cc", 8, &CPU::ret_cc};
    case 4:
      return {"LDH (a8) A", 12, &CPU::ldh_a8_a};
    case 5:
      return {"ADD SP e8", 16, &CPU::add_sp_e8};
    case 6:
      return {"LDH A (a8)", 12, &CPU::ldh_a_a8};
    case 7:
      return {"LD HL SP+e8", 12, &CPU::ld_hl_sp_e8};
    default:
      UNREACHABLE;
    }

  case 1:
    switch (op.y()) {
    case 0:
    case 2:
    case 4:
      return {"POP r16", 12, &CPU::pop_r16};
    case 6:
      return {"POP AF", 12, &CPU::pop_af};
    case 1:
      return {"RET", 16, &CPU::ret};
    case 3:
      return {"RETI", 16, &CPU::reti};
    case 5:
      return {"JP HL", 4, &CPU::jp_hl};
    case 7:
      return {"LD SP HL", 8, &CPU::ld_sp_hl};
    default:
      UNREACHABLE;
    }

  case 2:
    switch (op.y()) {
    case 0:
    case 1:
    case 2:
    case 3:
      return {"JP cc n16", 12, &CPU::jp_cc_a16};
    case 4:
      return {"LDH (C) A", 8, &CPU::ldh_mem_c_a};
    case 5:
      return {"LD (n16) A", 16, &CPU::ld_mem_n16_a};
    case 6:
      return {"LDH A (C)", 8, &CPU::ldh_a_mem_c};
    case 7:
      return {"LD A (n16)", 16, &CPU::ld_a_mem_n16};
    default:
      UNREACHABLE;
    }

  case 3:
    switch (op.y()) {
    case 0:
      return {"JP a16", 16, &CPU::jp_a16};
    case 1:
      return {"PREFIX", 4, &CPU::prefix};
    case 2:
    case 3:
    case 4:
    case 5:
      return {"XXX", 0, &CPU::illegal};
    case 6:
      return {"DI", 4, &CPU::di};
    case 7:
      return {"EI", 4, &CPU::ei};
    default:
      UNREACHABLE;
    }

  case 4:
    if (op.y() < 4)
      return {"CALL cc a16", 12, &CPU::call_cc_a16};
    else
      return {"XXX", 0, &CPU::illegal};

  case 5:
    switch (op.y()) {
    case 0:
    case 2:
    case 4:
      return {"PUSH r16", 16, &CPU::push_r16};
    case 6:
      return {"PUSH AF", 16, &CPU::push_af};
    case 1:
      return {"CALL a16", 24, &CPU::call_a16};
    case 3:
    case 5:
    case 7:
      return {"XXX", 0, &CPU::illegal};
    default:
      UNREACHABLE;
    }

  case 6:
    switch (op.y()) {
    case 0:
      return {"ADD n8", 8, &CPU::add_n8};
    case 1:
      return {"ADC n8", 8, &CPU::adc_n8};
    case 2:
      return {"SUB n8", 8, &CPU::sub_n8};
    case 3:
      return {"SBC n8", 8, &CPU::sbc_n8};
    case 4:
      return {"AND n8", 8, &CPU::and_n8};
    case 5:
      return {"XOR n8", 8, &CPU::xor_n8};
    case 6:
      return {"OR n8", 8, &CPU::or_n8};
    case 7:
      return {"CP n8", 8, &CPU::cp_n8};
    default:
      UNREACHABLE;
    }

  case 7:
    return {"RST", 16, &CPU::rst_vec};

  default:
    UNREACHABLE;
  }
}

auto CPU::block0_cb_ins(Opcode op) -> Instruction {
  switch (op.y()) {
  // RLC
  case 0:
    if (is_hl_ind(op.z())) {
      return {"RLC (HL)", 16, &CPU::rlc_mem_hl};
    } else {
      return {"RLC r8", 8, &CPU::rlc_r8};
    }

  // RRC
  case 1:
    if (is_hl_ind(op.z())) {
      return {"RRC (HL)", 16, &CPU::rrc_mem_hl};
    } else {
      return {"RRC r8", 8, &CPU::rrc_r8};
    }

  // RL
  case 2:
    if (is_hl_ind(op.z())) {
      return {"RL (HL)", 16, &CPU::rl_mem_hl};
    } else {
      return {"RL r8", 8, &CPU::rl_r8};
    }

  // RR
  case 3:
    if (is_hl_ind(op.z())) {
      return {"RR (HL)", 16, &CPU::rr_mem_hl};
    } else {
      return {"RR r8", 8, &CPU::rr_r8};
    }

  // SLA
  case 4:
    if (is_hl_ind(op.z())) {
      return {"SLA (HL)", 16, &CPU::sla_mem_hl};
    } else {
      return {"SLA r8", 8, &CPU::sla_r8};
    }

  // SRA
  case 5:
    if (is_hl_ind(op.z())) {
      return {"SRA (HL)", 16, &CPU::sra_mem_hl};
    } else {
      return {"SRA r8", 8, &CPU::sra_r8};
    }

  // SWAP
  case 6:
    if (is_hl_ind(op.z())) {
      return {"SWAP (HL)", 16, &CPU::swap_mem_hl};
    } else {
      return {"SWAP r8", 8, &CPU::swap_r8};
    }

  // SRL
  case 7:
    if (is_hl_ind(op.z())) {
      return {"SRL (HL)", 16, &CPU::srl_mem_hl};
    } else {
      return {"SRL r8", 8, &CPU::srl_r8};
    }

  default:
    UNREACHABLE;
  }
}

auto CPU::block1_cb_ins(Opcode op) -> Instruction {
  if (is_hl_ind(op.z())) {
    return {"BIT b3 (HL)", 12, &CPU::bit_mem_hl};
  } else {
    return {"BIT b3 r8", 8, &CPU::bit_r8};
  }
}

auto CPU::block2_cb_ins(Opcode op) -> Instruction {
  if (is_hl_ind(op.z())) {
    return {"RES b3 (HL)", 16, &CPU::res_mem_hl};
  } else {
    return {"RES b3 r8", 8, &CPU::res_r8};
  }
}

auto CPU::block3_cb_ins(Opcode op) -> Instruction {
  if (is_hl_ind(op.z())) {
    return {"SET b3 (HL)", 16, &CPU::set_mem_hl};
  } else {
    return {"SET b3 r8", 8, &CPU::set_r8};
  }
}
