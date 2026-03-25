#pragma once
#include "Bus.hpp"
#include "bit.hpp"
#include <array>
#include <cassert>
#include <cstdint>

class CPU {
public:
  union RegisterPair {
    struct {
      uint8_t lo, hi;
    } byte;
    uint16_t word = 0;
  };

  struct Opcode {
    uint8_t val;
    uint8_t hi;
    uint8_t mid;
    uint8_t lo;

    Opcode() : val(0), hi(0), mid(0), lo(0) {}

    Opcode(uint8_t op)
        : val(op), hi((op >> 6) & 0x3), mid((op >> 3) & 0x7), lo(op & 0x7) {}
  };

  struct Instruction {
    Opcode op;
    void (CPU::*exec)();
    const char *name;
    uint8_t cycles;
  };

  explicit CPU(Bus &bus);

  uint8_t step();

  [[nodiscard]] bool is_running() const { return state == State::Running; }

private: // Registers and state
  enum class State : uint8_t { Running, Halt, Stop };

  State state{State::Running};

  Bus &bus;

  uint8_t extra_cycles = 0;

  Opcode opcode = 0;

  RegisterPair BC{}, DE{}, HL{};

  uint8_t A = 0, F = 0;

  uint16_t PC = 0x100, SP = 0;

  bool IME = false;

private: // Flags
  enum class Flag : uint8_t { C = 4, H, N, Z };

  void set_flag(Flag flag, bool val) {
    bit::set(F, static_cast<uint8_t>(flag), val);
  }

  [[nodiscard]] uint8_t get_flag(Flag flag) const {
    return bit::get(F, static_cast<uint8_t>(flag));
  }

  [[nodiscard]] bool is_flag_set(Flag flag) const {
    return bit::is_set(F, static_cast<uint8_t>(flag));
  }

private: // Static lookup tables
  const std::array<uint8_t *, 8> r8{&BC.byte.hi, &BC.byte.lo, &DE.byte.hi,
                                    &DE.byte.lo, &HL.byte.hi, &HL.byte.lo,
                                    nullptr,     &A};

  const std::array<uint16_t *, 4> r16{&BC.word, &DE.word, &HL.word, &SP};

private: // Lookup and decoding
  [[nodiscard]] bool cond(uint8_t mid) const;

private: // Utility functions
  void log_ins(const Instruction &ins) const;

private: // 8-bit read and writes
  [[nodiscard]] uint8_t fetch_byte() { return bus.read(PC++); }

  [[nodiscard]] uint16_t fetch_word();

  [[nodiscard]] uint8_t read_hl() const { return bus.read(HL.word); }

  void write_hl(const uint8_t val) { bus.write(HL.word, val); }

private: // 16_bit read and writes
  [[nodiscard]] uint16_t read_word(const uint16_t addr) const {
    return static_cast<uint16_t>(bus.read(addr) | bus.read(addr + 1) << 8);
  }

  void write_word(uint16_t address, uint16_t value);

private: // Instruction implementations
  void impl_ldh_r8_a8(uint8_t &dest, uint8_t addr) {
    dest = bus.read(static_cast<uint16_t>(addr) + 0xFF00);
  }

  void impl_ldh_a8_r8(uint8_t addr, uint8_t r8) {
    bus.write(static_cast<uint16_t>(addr) + 0xFF00, r8);
  }

  [[nodiscard]] uint8_t impl_adc_n8(uint8_t n8);

  [[nodiscard]] uint8_t impl_add_n8(uint8_t n8);

  [[nodiscard]] uint16_t impl_add_n16(uint16_t n16);

  [[nodiscard]] uint16_t impl_add_sp_e8();

  [[nodiscard]] uint8_t impl_dec_n8(uint8_t n8);

  [[nodiscard]] uint8_t impl_inc_n8(uint8_t n8);

  [[nodiscard]] uint8_t impl_sbc_n8(uint8_t n8);

  [[nodiscard]] uint8_t impl_sub_n8(uint8_t n8);

  [[nodiscard]] uint8_t impl_and_n8(uint8_t n8);

  [[nodiscard]] uint8_t impl_or_n8(uint8_t n8);

  [[nodiscard]] uint8_t impl_xor_n8(uint8_t n8);

  void impl_bit_n8(const uint8_t bit_idx, const uint8_t n8) {
    set_flag(Flag::Z, !bit::is_set(n8, bit_idx));
    set_flag(Flag::N, false);
    set_flag(Flag::H, true);
  }

  enum class Prefix : uint8_t { CB, None };

  [[nodiscard]] uint8_t impl_rl_n8(uint8_t n8, Prefix prefix);

  [[nodiscard]] uint8_t impl_rlc_n8(uint8_t n8, Prefix prefix);

  [[nodiscard]] uint8_t impl_rr_n8(uint8_t n8, Prefix prefix);

  [[nodiscard]] uint8_t impl_rrc_n8(uint8_t n8, Prefix prefix);

  [[nodiscard]] uint8_t impl_sla_n8(uint8_t n8);

  [[nodiscard]] uint8_t impl_sra_n8(uint8_t n8);

  [[nodiscard]] uint8_t impl_srl_n8(uint8_t n8);

  [[nodiscard]] uint8_t impl_swap_n8(uint8_t n8);

private: // Load instructions
  // 8-bit register loads
  void ld_r8_r8() { *r8[opcode.mid] = *r8[opcode.lo]; }

  void ld_r8_mem_hl() { *r8[opcode.mid] = read_hl(); }

  void ld_r8_n8() { *r8[opcode.mid] = fetch_byte(); }

  void ld_a_mem_r16() { A = bus.read(*r16[opcode.mid >> 1]); }

  void ld_a_a16() { A = bus.read(fetch_word()); }

  void ld_a_mem_hli() { A = bus.read(HL.word++); }

  void ld_a_mem_hld() { A = bus.read(HL.word--); }

  void ldh_a_a8() { impl_ldh_r8_a8(A, fetch_byte()); }

  void ldh_a_mem_c() { impl_ldh_r8_a8(A, BC.byte.lo); }

  // 16-bit register loads
  void ld_r16_n16() { *r16[opcode.mid >> 1] = fetch_word(); }

  // 16-bit address writes
  void ld_mem_hl_r8() { write_hl(*r8[opcode.lo]); }

  void ld_mem_hl_n8() { write_hl(fetch_byte()); }

  void ld_mem_hli_a() { bus.write(HL.word++, A); }

  void ld_mem_hld_a() { bus.write(HL.word--, A); }

  void ld_mem_r16_a() { bus.write(*r16[opcode.mid >> 1], A); }

  void ld_a16_a() { bus.write(fetch_word(), A); }

  // 8-bit address writes
  void ldh_a8_a() { impl_ldh_a8_r8(fetch_byte(), A); }

  void ldh_mem_c_a() { impl_ldh_a8_r8(BC.byte.lo, A); }

private: // Arithmetic
  // ADC
  void adc_r8() { A = impl_adc_n8(*r8[opcode.lo]); }

  void adc_mem_hl() { A = impl_adc_n8(read_hl()); }

  void adc_imm8() { A = impl_adc_n8(fetch_byte()); }

  // ADD
  void add_r8() { A = impl_add_n8(*r8[opcode.lo]); }

  void add_mem_hl() { A = impl_add_n8(read_hl()); }

  void add_n8() { A = impl_add_n8(fetch_byte()); }

  void add_hl_r16() { HL.word = impl_add_n16(*r16[opcode.mid >> 1]); }

  // CP
  void cp_r8() { static_cast<void>(impl_sub_n8(*r8[opcode.lo])); }

  void cp_mem_hl() { static_cast<void>(impl_sub_n8(read_hl())); }

  void cp_imm8() { static_cast<void>(impl_sub_n8(fetch_byte())); }

  // DEC
  void dec_r8() { *r8[opcode.lo] = impl_dec_n8(*r8[opcode.lo]); }

  void dec_mem_hl() { write_hl(impl_dec_n8(read_hl())); }

  void dec_r16() { --*r16[opcode.mid >> 1]; }

  // INC
  void inc_r8() { *r8[opcode.mid] = impl_inc_n8(*r8[opcode.mid]); }

  void inc_mem_hl() { write_hl(impl_inc_n8(read_hl())); }

  void inc_r16() { ++*r16[opcode.mid >> 1]; };

  // SBC
  void sbc_r8() { A = impl_sbc_n8(*r8[opcode.lo]); }

  void sbc_mem_hl() { A = impl_sbc_n8(read_hl()); }

  void sbc_imm8() { A = impl_sbc_n8(fetch_byte()); }

  // SUB
  void sub_r8() { A = impl_sub_n8(*r8[opcode.lo]); }

  void sub_mem_hl() { A = impl_sub_n8(read_hl()); }

  void sub_imm8() { A = impl_sub_n8(fetch_byte()); }

private: // Bitwise logic
  // AND
  void and_r8() { A = impl_and_n8(*r8[opcode.lo]); }

  void and_mem_hl() { A = impl_and_n8(read_hl()); }

  void and_imm8() { A = impl_and_n8(fetch_byte()); }

  // OR
  void or_r8() { A = impl_or_n8(*r8[opcode.lo]); }

  void or_mem_hl() { A = impl_or_n8(read_hl()); };

  void or_imm8() { A = impl_or_n8(fetch_byte()); }

  // XOR
  void xor_r8() { A = impl_xor_n8(*r8[opcode.lo]); }

  void xor_mem_hl() { A = impl_xor_n8(read_hl()); }

  void xor_imm8() { A = impl_xor_n8(fetch_byte()); }

  // CPL
  void cpl() {
    A = static_cast<uint8_t>(~A);
    set_flag(Flag::N, true);
    set_flag(Flag::H, true);
  }

private: // Bit flag
  // BIT
  void bit_r8() { impl_bit_n8(opcode.mid, *r8[opcode.lo]); }

  void bit_mem_hl() { impl_bit_n8(opcode.mid, read_hl()); }

  // RES
  void res_r8() { bit::set(*r8[opcode.lo], opcode.mid, false); }

  void res_mem_hl();

  // SET
  void set_r8() { bit::set(*r8[opcode.lo], opcode.mid, true); }

  void set_mem_hl();

private: // Bit shift
  // RL
  void rl_r8() { *r8[opcode.lo] = impl_rl_n8(*r8[opcode.lo], Prefix::CB); }

  void rl_mem_hl() { write_hl(impl_rl_n8(read_hl(), Prefix::CB)); }

  void rla() { A = impl_rl_n8(A, Prefix::None); }

  // RLC
  void rlc_r8() { *r8[opcode.lo] = impl_rlc_n8(*r8[opcode.lo], Prefix::CB); }

  void rlc_mem_hl() { write_hl(impl_rlc_n8(read_hl(), Prefix::CB)); }

  void rlca() { A = impl_rlc_n8(A, Prefix::None); }

  // RR
  void rr_r8() { *r8[opcode.lo] = impl_rr_n8(*r8[opcode.lo], Prefix::CB); }

  void rr_mem_hl() { write_hl(impl_rr_n8(read_hl(), Prefix::CB)); }

  void rra() { A = impl_rr_n8(A, Prefix::None); }

  // RRC
  void rrc_r8() { *r8[opcode.lo] = impl_rrc_n8(*r8[opcode.lo], Prefix::CB); }

  void rrc_mem_hl() { write_hl(impl_rrc_n8(read_hl(), Prefix::CB)); }

  void rrca() { A = impl_rrc_n8(A, Prefix::None); }

  // SLA
  void sla_r8() { *r8[opcode.lo] = impl_sla_n8(*r8[opcode.lo]); }

  void sla_mem_hl() { write_hl(impl_sla_n8(read_hl())); }

  // SRA
  void sra_r8() { *r8[opcode.lo] = impl_sra_n8(*r8[opcode.lo]); }

  void sra_mem_hl() { write_hl(impl_sra_n8(read_hl())); }

  // SRL
  void srl_r8() { *r8[opcode.lo] = impl_srl_n8(*r8[opcode.lo]); }

  void srl_mem_hl() { write_hl(impl_srl_n8(read_hl())); }

  // SWAP
  void swap_r8() { *r8[opcode.lo] = impl_swap_n8(*r8[opcode.lo]); }

  void swap_mem_hl() { write_hl(impl_swap_n8(read_hl())); }

private: // Jumps and subroutine
  void jp_hl() { PC = HL.word; }

  void jp_a16() { PC = fetch_word(); }

  void jp_cc_a16() {
    uint16_t addr = fetch_word();
    if (cond(opcode.mid)) {
      PC = addr;
      extra_cycles = 4;
    }
  }

  void jr_e8() {
    PC = static_cast<uint16_t>(PC + static_cast<int8_t>(fetch_byte()));
  }

  void jr_cc_e8() {
    auto addr = static_cast<uint16_t>(PC + static_cast<int8_t>(fetch_byte()));
    if (cond(opcode.mid)) {
      PC = addr;
      extra_cycles = 4;
    }
  }

  void call_a16() {
    SP -= 2;
    write_word(SP, PC);
    PC = fetch_word();
  }

  void call_cc_a16();

  void ret() {
    PC = read_word(SP);
    SP += 2;
  }

  void ret_cc() {
    uint16_t addr = read_word(SP);
    if (cond(opcode.mid)) {
      PC = addr;
      SP += 2;
      extra_cycles = 12;
    }
  }

  void reti() {
    PC = read_word(SP);
    SP += 2;
    IME = true;
  }

  void rst_vec() {
    SP -= 2;
    write_word(SP, PC);
    PC = static_cast<uint16_t>(opcode.mid) * 8;
  }

private: // Carry flag
  void ccf() {
    set_flag(Flag::Z, false);
    set_flag(Flag::H, false);
    set_flag(Flag::C, !is_flag_set(Flag::C));
  }

  void scf() {
    set_flag(Flag::Z, false);
    set_flag(Flag::H, false);
    set_flag(Flag::C, true);
  }

private: // Stack manipulation
  void add_sp_e8() { SP = impl_add_sp_e8(); }

  void ld_hl_sp_e8() { HL.word = impl_add_sp_e8(); }

  void ld_sp_hl() { SP = HL.word; }

  void ld_a16_sp() { write_word(fetch_word(), SP); }

  void pop_r16() {
    *r16[opcode.mid >> 1] = read_word(SP);
    SP += 2;
  }

  void pop_af() {
    F = bus.read(SP++);
    A = bus.read(SP++);
  }

  void push_r16() {
    SP -= 2;
    write_word(SP, *r16[opcode.mid >> 1]);
  }

  void push_af() {
    bus.write(--SP, A);
    bus.write(--SP, F);
  }

private: // Interrupt-related
  void di() { IME = false; }

  void ei() { IME = true; }

  void halt() { state = State::Halt; }

private: // Misc.
  void nop() {}

  void stop() { state = State::Stop; }

  void daa();

  void prefix();

  void illegal();

private: // Instruction Set
  static Instruction block0_ins(Opcode op);
  static Instruction block1_ins(Opcode op);
  static Instruction block2_ins(Opcode op);
  static Instruction block3_ins(Opcode op);

  static inline const std::array<Instruction, 0x100> optable = [] {
    std::array<Instruction, 0x100> ins{};

    for (size_t i = 0x0; i < ins.size(); ++i) {
      auto opcode = Opcode(static_cast<uint8_t>(i));

      switch (opcode.hi) {
      case 0:
        ins[i] = block0_ins(opcode);
        break;
      case 1:
        ins[i] = block1_ins(opcode);
        break;
      case 2:
        ins[i] = block2_ins(opcode);
        break;
      case 3:
        ins[i] = block3_ins(opcode);
        break;
      default:
        assert(false);
      }
    }

    return ins;
  }();

  static Instruction block0_cb_ins(Opcode op);
  static Instruction block1_cb_ins(Opcode op);
  static Instruction block2_cb_ins(Opcode op);
  static Instruction block3_cb_ins(Opcode op);

  static inline const std::array<Instruction, 0x100> cb_optable = [] {
    std::array<Instruction, 0x100> ins{};

    for (size_t i = 0x0; i < ins.size(); ++i) {
      auto opcode = Opcode(static_cast<uint8_t>(i));

      switch (opcode.hi) {
      case 0:
        ins[i] = block0_cb_ins(opcode);
        break;
      case 1:
        ins[i] = block1_cb_ins(opcode);
        break;
      case 2:
        ins[i] = block2_cb_ins(opcode);
        break;
      case 3:
        ins[i] = block3_cb_ins(opcode);
        break;
      default:
        assert(false);
      }
    }

    return ins;
  }();
};
