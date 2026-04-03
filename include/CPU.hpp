#pragma once
#include "Bus.hpp"
#include "CPU_Types.hpp"
#include "bit.hpp"
#include <array>
#include <cassert>
#include <cstdint>

class CPU {
public:
  explicit CPU(Bus &bus);

  auto step() -> uint8_t;

  [[nodiscard]] auto is_running() const -> bool {
    return state == State::Running;
  }

private: // Registers and state
  enum class State : uint8_t { Running, Halted, Stopped };

  State state{State::Running};

  Bus &bus;

  uint8_t extra_cycles = 0;

  uint8_t A = 0;
  Flags F = {};

  uint16_t PC = 0x100, SP = 0;

  RegisterPair BC{}, DE{}, HL{};

  Opcode opcode = {};

  bool IME = false;

private: // Register lookups
  const std::array<uint8_t *, 8> r8{&BC.byte.hi, &BC.byte.lo, &DE.byte.hi,
                                    &DE.byte.lo, &HL.byte.hi, &HL.byte.lo,
                                    nullptr,     &A};

  const std::array<uint16_t *, 4> r16{&BC.word, &DE.word, &HL.word, &SP};

private: // Flags
private: // Lookup and decoding
  [[nodiscard]] auto cond(uint8_t y) const -> bool;

private: // Utility functions
  void log_ins(const Instruction &ins) const;

private: // Memory read and writes
  [[nodiscard]] auto fetch_byte() -> uint8_t { return bus.read(PC++); }

  [[nodiscard]] auto read_hl() const -> uint8_t { return bus.read(HL.word); }

  void write_hl(const uint8_t val) { bus.write(HL.word, val); }

  [[nodiscard]] auto read_word(uint16_t addr) const -> uint16_t;

  [[nodiscard]] auto fetch_word() -> uint16_t {
    const auto word = read_word(PC);
    PC += 2;
    return word;
  };

  void write_word(uint16_t address, uint16_t value);

private: // Instruction implementations
  enum class Prefix : uint8_t { CB, None };

  [[nodiscard]] auto impl_adc_n8(uint8_t n8) -> uint8_t;

  [[nodiscard]] auto impl_add_n8(uint8_t n8) -> uint8_t;

  [[nodiscard]] auto impl_add_n16(uint16_t n16) -> uint16_t;

  [[nodiscard]] auto impl_add_sp_e8() -> uint16_t;

  [[nodiscard]] auto impl_dec_n8(uint8_t n8) -> uint8_t;

  [[nodiscard]] auto impl_inc_n8(uint8_t n8) -> uint8_t;

  [[nodiscard]] auto impl_sbc_n8(uint8_t n8) -> uint8_t;

  [[nodiscard]] auto impl_sub_n8(uint8_t n8) -> uint8_t;

  [[nodiscard]] auto impl_and_n8(uint8_t n8) -> uint8_t;

  [[nodiscard]] auto impl_or_n8(uint8_t n8) -> uint8_t;

  [[nodiscard]] auto impl_xor_n8(uint8_t n8) -> uint8_t;

  [[nodiscard]] auto impl_rl_n8(uint8_t n8, Prefix prefix) -> uint8_t;

  [[nodiscard]] auto impl_rlc_n8(uint8_t n8, Prefix prefix) -> uint8_t;

  [[nodiscard]] auto impl_rr_n8(uint8_t n8, Prefix prefix) -> uint8_t;

  [[nodiscard]] auto impl_rrc_n8(uint8_t n8, Prefix prefix) -> uint8_t;

  [[nodiscard]] auto impl_sla_n8(uint8_t n8) -> uint8_t;

  [[nodiscard]] auto impl_sra_n8(uint8_t n8) -> uint8_t;

  [[nodiscard]] auto impl_srl_n8(uint8_t n8) -> uint8_t;

  [[nodiscard]] auto impl_swap_n8(uint8_t n8) -> uint8_t;

  void impl_bit_n8(const uint8_t bit_idx, const uint8_t n8) {
    F.set(Flags::Z, !bit::get(n8, bit_idx));
    F.set(Flags::N, false);
    F.set(Flags::H, true);
  }

  void impl_ldh_r8_a8(uint8_t &dest, uint8_t addr) {
    dest = bus.read(static_cast<uint16_t>(addr) + 0xFF00);
  }

  void impl_ldh_a8_r8(uint8_t addr, uint8_t reg) {
    bus.write(static_cast<uint16_t>(addr) + 0xFF00, reg);
  }

private: // Load instructions
  // 8-bit register loads
  void ld_r8_r8() { *r8[opcode.y()] = *r8[opcode.z()]; }

  void ld_r8_mem_hl() { *r8[opcode.y()] = read_hl(); }

  void ld_r8_n8() { *r8[opcode.y()] = fetch_byte(); }

  void ld_a_mem_r16() { A = bus.read(*r16[opcode.y() >> 1]); }

  void ld_a_a16() { A = bus.read(fetch_word()); }

  void ld_a_mem_hli() { A = bus.read(HL.word++); }

  void ld_a_mem_hld() { A = bus.read(HL.word--); }

  void ldh_a_a8() { impl_ldh_r8_a8(A, fetch_byte()); }

  void ldh_a_mem_c() { impl_ldh_r8_a8(A, BC.byte.lo); }

  // 16-bit register loads
  void ld_r16_n16() { *r16[opcode.y() >> 1] = fetch_word(); }

  // 16-bit address writes
  void ld_mem_hl_r8() { write_hl(*r8[opcode.z()]); }

  void ld_mem_hl_n8() { write_hl(fetch_byte()); }

  void ld_mem_hli_a() { bus.write(HL.word++, A); }

  void ld_mem_hld_a() { bus.write(HL.word--, A); }

  void ld_mem_r16_a() { bus.write(*r16[opcode.y() >> 1], A); }

  void ld_a16_a() { bus.write(fetch_word(), A); }

  // 8-bit address writes
  void ldh_a8_a() { impl_ldh_a8_r8(fetch_byte(), A); }

  void ldh_mem_c_a() { impl_ldh_a8_r8(BC.byte.lo, A); }

private: // Arithmetic
  // ADC
  void adc_r8() { A = impl_adc_n8(*r8[opcode.z()]); }

  void adc_mem_hl() { A = impl_adc_n8(read_hl()); }

  void adc_imm8() { A = impl_adc_n8(fetch_byte()); }

  // ADD
  void add_r8() { A = impl_add_n8(*r8[opcode.z()]); }

  void add_mem_hl() { A = impl_add_n8(read_hl()); }

  void add_n8() { A = impl_add_n8(fetch_byte()); }

  void add_hl_r16() { HL.word = impl_add_n16(*r16[opcode.y() >> 1]); }

  // CP
  void cp_r8() { (void)impl_sub_n8(*r8[opcode.z()]); }

  void cp_mem_hl() { (void)impl_sub_n8(read_hl()); }

  void cp_imm8() { (void)impl_sub_n8(fetch_byte()); }

  // DEC
  void dec_r8() { *r8[opcode.z()] = impl_dec_n8(*r8[opcode.z()]); }

  void dec_mem_hl() { write_hl(impl_dec_n8(read_hl())); }

  void dec_r16() { --*r16[opcode.y() >> 1]; }

  // INC
  void inc_r8() { *r8[opcode.y()] = impl_inc_n8(*r8[opcode.y()]); }

  void inc_mem_hl() { write_hl(impl_inc_n8(read_hl())); }

  void inc_r16() { ++*r16[opcode.y() >> 1]; };

  // SBC
  void sbc_r8() { A = impl_sbc_n8(*r8[opcode.z()]); }

  void sbc_mem_hl() { A = impl_sbc_n8(read_hl()); }

  void sbc_imm8() { A = impl_sbc_n8(fetch_byte()); }

  // SUB
  void sub_r8() { A = impl_sub_n8(*r8[opcode.z()]); }

  void sub_mem_hl() { A = impl_sub_n8(read_hl()); }

  void sub_imm8() { A = impl_sub_n8(fetch_byte()); }

private: // Bitwise logic
  // AND
  void and_r8() { A = impl_and_n8(*r8[opcode.z()]); }

  void and_mem_hl() { A = impl_and_n8(read_hl()); }

  void and_imm8() { A = impl_and_n8(fetch_byte()); }

  // OR
  void or_r8() { A = impl_or_n8(*r8[opcode.z()]); }

  void or_mem_hl() { A = impl_or_n8(read_hl()); };

  void or_imm8() { A = impl_or_n8(fetch_byte()); }

  // XOR
  void xor_r8() { A = impl_xor_n8(*r8[opcode.z()]); }

  void xor_mem_hl() { A = impl_xor_n8(read_hl()); }

  void xor_imm8() { A = impl_xor_n8(fetch_byte()); }

  // CPL
  void cpl();

private: // Bit flag
  // BIT
  void bit_r8() { impl_bit_n8(opcode.y(), *r8[opcode.z()]); }

  void bit_mem_hl() { impl_bit_n8(opcode.y(), read_hl()); }

  // RES
  void res_r8() { bit::set(*r8[opcode.z()], opcode.y(), false); }

  void res_mem_hl();

  // SET
  void set_r8() { bit::set(*r8[opcode.z()], opcode.y(), true); }

  void set_mem_hl();

private: // Bit shift
  // RL
  void rl_r8() { *r8[opcode.z()] = impl_rl_n8(*r8[opcode.z()], Prefix::CB); }

  void rl_mem_hl() { write_hl(impl_rl_n8(read_hl(), Prefix::CB)); }

  void rla() { A = impl_rl_n8(A, Prefix::None); }

  // RLC
  void rlc_r8() { *r8[opcode.z()] = impl_rlc_n8(*r8[opcode.z()], Prefix::CB); }

  void rlc_mem_hl() { write_hl(impl_rlc_n8(read_hl(), Prefix::CB)); }

  void rlca() { A = impl_rlc_n8(A, Prefix::None); }

  // RR
  void rr_r8() { *r8[opcode.z()] = impl_rr_n8(*r8[opcode.z()], Prefix::CB); }

  void rr_mem_hl() { write_hl(impl_rr_n8(read_hl(), Prefix::CB)); }

  void rra() { A = impl_rr_n8(A, Prefix::None); }

  // RRC
  void rrc_r8() { *r8[opcode.z()] = impl_rrc_n8(*r8[opcode.z()], Prefix::CB); }

  void rrc_mem_hl() { write_hl(impl_rrc_n8(read_hl(), Prefix::CB)); }

  void rrca() { A = impl_rrc_n8(A, Prefix::None); }

  // SLA
  void sla_r8() { *r8[opcode.z()] = impl_sla_n8(*r8[opcode.z()]); }

  void sla_mem_hl() { write_hl(impl_sla_n8(read_hl())); }

  // SRA
  void sra_r8() { *r8[opcode.z()] = impl_sra_n8(*r8[opcode.z()]); }

  void sra_mem_hl() { write_hl(impl_sra_n8(read_hl())); }

  // SRL
  void srl_r8() { *r8[opcode.z()] = impl_srl_n8(*r8[opcode.z()]); }

  void srl_mem_hl() { write_hl(impl_srl_n8(read_hl())); }

  // SWAP
  void swap_r8() { *r8[opcode.z()] = impl_swap_n8(*r8[opcode.z()]); }

  void swap_mem_hl() { write_hl(impl_swap_n8(read_hl())); }

private: // Jumps and subroutine
  void jp_hl() { PC = HL.word; }

  void jp_a16() { PC = fetch_word(); }

  void jp_cc_a16();

  void jr_e8() {
    PC = static_cast<uint16_t>(PC + static_cast<int8_t>(fetch_byte()));
  }

  void jr_cc_e8();

  void call_a16();

  void call_cc_a16();

  void ret() {
    PC = read_word(SP);
    SP += 2;
  }

  void ret_cc();

  void reti();

  void rst_vec();

private: // Carry flag
  void ccf();

  void scf();

private: // Stack manipulation
  void add_sp_e8() { SP = impl_add_sp_e8(); }

  void ld_hl_sp_e8() { HL.word = impl_add_sp_e8(); }

  void ld_sp_hl() { SP = HL.word; }

  void ld_a16_sp() { write_word(fetch_word(), SP); }

  void pop_r16() {
    *r16[opcode.y() >> 1] = read_word(SP);
    SP += 2;
  }

  void pop_af() {
    F.byte = bus.read(SP++);
    A = bus.read(SP++);
  }

  void push_r16() {
    SP -= 2;
    write_word(SP, *r16[opcode.y() >> 1]);
  }

  void push_af() {
    bus.write(--SP, A);
    bus.write(--SP, F.byte);
  }

private: // Interrupt-related
  void di() { IME = false; }

  void ei() { IME = true; }

  void halt() { state = State::Halted; }

private: // Misc.
  void nop() {}

  void stop() { state = State::Stopped; }

  void daa();

  void prefix();

  void illegal();

private: // Instruction Set
  static auto block0_ins(Opcode op) -> Instruction;
  static auto block1_ins(Opcode op) -> Instruction;
  static auto block2_ins(Opcode op) -> Instruction;
  static auto block3_ins(Opcode op) -> Instruction;

  static inline const std::array<Instruction, 0x100> optable = [] {
    std::array<Instruction, 0x100> ins{};

    for (size_t i = 0x0; i < ins.size(); ++i) {
      auto opcode = Opcode(static_cast<uint8_t>(i));

      switch (opcode.x()) {
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

  static auto block0_cb_ins(Opcode op) -> Instruction;
  static auto block1_cb_ins(Opcode op) -> Instruction;
  static auto block2_cb_ins(Opcode op) -> Instruction;
  static auto block3_cb_ins(Opcode op) -> Instruction;

  static inline const std::array<Instruction, 0x100> cb_optable = [] {
    std::array<Instruction, 0x100> ins{};

    for (size_t i = 0x0; i < ins.size(); ++i) {
      const auto opcode = Opcode(static_cast<uint8_t>(i));

      switch (opcode.x()) {
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
