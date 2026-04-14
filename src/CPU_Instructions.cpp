#include "CPU.hpp"
#include "bit.hpp"
#include <cstdint>
#include <iostream>

auto CPU::impl_adc_n8(const uint8_t n8) -> uint8_t {
  const auto sum = A + n8 + F.get(Flags::C);
  const auto result = static_cast<uint8_t>(sum);

  F.set(Flags::Z, result == 0);
  F.set(Flags::N, false);
  F.set(Flags::H, (A & 0xF) + (n8 & 0xF) + F.get(Flags::C) > 0xF);
  F.set(Flags::C, sum > 0xFF);

  return result;
}

auto CPU::impl_add_n8(const uint8_t n8) -> uint8_t {
  const auto sum = A + n8;
  const auto result = static_cast<uint8_t>(sum);

  F.set(Flags::Z, result == 0);
  F.set(Flags::N, false);
  F.set(Flags::H, (A & 0xF) + (n8 & 0xF) > 0xF);
  F.set(Flags::C, sum > 0xFF);

  return result;
}

auto CPU::impl_add_n16(const uint16_t n16) -> uint16_t {
  const auto sum = HL.word + n16;

  F.set(Flags::N, false);
  F.set(Flags::H, (HL.word & 0xFFF) + (n16 & 0xFFF) > 0xFFF);
  F.set(Flags::C, sum > 0xFFFF);

  return static_cast<uint16_t>(sum);
}

auto CPU::impl_add_sp_e8() -> uint16_t {
  const auto e8 = static_cast<int8_t>(fetch_byte());
  const auto result = static_cast<uint16_t>(SP + e8);

  F.set(Flags::Z, false);
  F.set(Flags::N, false);
  F.set(Flags::H, (SP & 0xF) + (e8 & 0xF) > 0xF);
  F.set(Flags::C, (SP & 0xFF) + e8 > 0xFF);

  return result;
}

auto CPU::impl_dec_n8(const uint8_t n8) -> uint8_t {
  const uint8_t result = n8 - 1;

  F.set(Flags::Z, result == 0);
  F.set(Flags::N, true);
  F.set(Flags::H, (n8 & 0xF) == 0);

  return result;
}

auto CPU::impl_inc_n8(const uint8_t n8) -> uint8_t {
  const uint8_t result = n8 + 1;

  F.set(Flags::Z, result == 0);
  F.set(Flags::N, false);
  F.set(Flags::H, (n8 & 0xF) == 0xF);

  return result;
}

auto CPU::impl_sbc_n8(const uint8_t n8) -> uint8_t {
  const auto carry = F.get(Flags::C);
  const uint8_t result = A - n8 - carry;

  F.set(Flags::Z, result == 0);
  F.set(Flags::N, true);
  F.set(Flags::H, (n8 & 0xF) + carry > (A & 0xF));
  F.set(Flags::C, n8 + carry > A);

  return result;
}

auto CPU::impl_sub_n8(const uint8_t n8) -> uint8_t {
  const uint8_t result = A - n8;

  F.set(Flags::Z, result == 0);
  F.set(Flags::N, true);
  F.set(Flags::H, (n8 & 0xF) > (A & 0xF));
  F.set(Flags::C, n8 > A);

  return result;
}

auto CPU::impl_and_n8(const uint8_t n8) -> uint8_t {
  const uint8_t result = A & n8;

  F.set(Flags::Z, result == 0);
  F.set(Flags::N, false);
  F.set(Flags::H, true);
  F.set(Flags::C, false);

  return result;
}

auto CPU::impl_or_n8(const uint8_t n8) -> uint8_t {
  const uint8_t result = A | n8;

  F.set(Flags::Z, result == 0);
  F.set(Flags::N, false);
  F.set(Flags::H, false);
  F.set(Flags::C, false);

  return result;
}

auto CPU::impl_xor_n8(const uint8_t n8) -> uint8_t {
  const uint8_t result = A ^ n8;

  F.set(Flags::Z, result == 0);
  F.set(Flags::N, false);
  F.set(Flags::H, false);
  F.set(Flags::C, false);

  return result;
}

auto CPU::impl_rl_n8(const uint8_t n8, const Prefix prefix) -> uint8_t {
  const uint8_t result = (n8 << 1) | F.get(Flags::C);

  F.set(Flags::Z, result == 0 && prefix == Prefix::CB);
  F.set(Flags::N, false);
  F.set(Flags::H, false);
  F.set(Flags::C, bit::get(n8, 7));

  return result;
}

auto CPU::impl_rlc_n8(const uint8_t n8, const Prefix prefix) -> uint8_t {
  const uint8_t result = n8 << 1 | bit::get(n8, 7);

  F.set(Flags::Z, result == 0 && prefix == Prefix::CB);
  F.set(Flags::N, false);
  F.set(Flags::H, false);
  F.set(Flags::C, bit::get(n8, 7));

  return result;
}

auto CPU::impl_rr_n8(const uint8_t n8, const Prefix prefix) -> uint8_t {
  const uint8_t result = n8 >> 1 | F.get(Flags::C) << 7;

  F.set(Flags::Z, result == 0 && prefix == Prefix::CB);
  F.set(Flags::N, false);
  F.set(Flags::H, false);
  F.set(Flags::C, (n8 & 1));

  return result;
}

auto CPU::impl_rrc_n8(const uint8_t n8, const Prefix prefix) -> uint8_t {
  const uint8_t result = n8 >> 1 | (n8 & 1) << 7;

  F.set(Flags::Z, result == 0 && prefix == Prefix::CB);
  F.set(Flags::N, false);
  F.set(Flags::H, false);
  F.set(Flags::C, (n8 & 1));

  return result;
}

auto CPU::impl_sla_n8(const uint8_t n8) -> uint8_t {
  const uint8_t result = n8 << 1;

  F.set(Flags::Z, result == 0);
  F.set(Flags::N, false);
  F.set(Flags::H, false);
  F.set(Flags::C, bit::get(n8, 7));

  return result;
}

auto CPU::impl_sra_n8(const uint8_t n8) -> uint8_t {
  const uint8_t result = n8 >> 1 | n8 & 0x80;

  F.set(Flags::Z, result == 0);
  F.set(Flags::N, false);
  F.set(Flags::H, false);
  F.set(Flags::C, (n8 & 1));

  return result;
}

auto CPU::impl_srl_n8(const uint8_t n8) -> uint8_t {
  const uint8_t result = n8 >> 1;

  F.set(Flags::Z, result == 0);
  F.set(Flags::N, false);
  F.set(Flags::H, false);
  F.set(Flags::C, (n8 & 1));

  return result;
}

auto CPU::impl_swap_n8(const uint8_t n8) -> uint8_t {
  const uint8_t hi = (n8 & 0xF) << 4;
  const uint8_t lo = (n8 & 0xF0) >> 4;
  const uint8_t result = hi | lo;

  F.set(Flags::Z, result == 0);
  F.set(Flags::N, false);
  F.set(Flags::H, false);
  F.set(Flags::C, false);

  return result;
}

void CPU::impl_bit_n8(const uint8_t bit_idx, const uint8_t n8) {
  F.set(Flags::Z, !bit::get(n8, bit_idx));
  F.set(Flags::N, false);
  F.set(Flags::H, true);
}

void CPU::cpl() {
  A = static_cast<uint8_t>(~A);
  F.set(Flags::N, true);
  F.set(Flags::H, true);
}

void CPU::res_mem_hl() {
  auto hl_ind = read_hl();
  bit::set(hl_ind, IR.y(), false);
  write_hl(hl_ind);
}

void CPU::set_mem_hl() {
  auto hl_ind = read_hl();
  bit::set(hl_ind, IR.y(), true);
  write_hl(hl_ind);
}

void CPU::jp_cc_a16() {
  const auto addr = fetch_word();
  if (cond(IR.y())) {
    PC = addr;
    extra_cycles = 4;
  }
}

void CPU::jr_cc_e8() {
  const auto addr = PC + static_cast<int8_t>(fetch_byte());
  if (cond(IR.y())) {
    PC = static_cast<uint16_t>(addr);
    extra_cycles = 4;
  }
}

void CPU::call_a16() {
  SP -= 2;
  write_word(SP, PC);
  PC = fetch_word();
}

void CPU::call_cc_a16() {
  const auto addr = fetch_word();
  if (cond(IR.y())) {
    SP -= 2;
    write_word(SP, PC);
    PC = addr;
    extra_cycles = 12;
  }
}

void CPU::ret_cc() {
  const auto addr = read_word(SP);
  if (cond(IR.y())) {
    PC = addr;
    SP += 2;
    extra_cycles = 12;
  }
}

void CPU::reti() {
  PC = read_word(SP);
  SP += 2;
  IME = true;
}

void CPU::rst_vec() {
  SP -= 2;
  write_word(SP, PC);
  PC = static_cast<uint16_t>(IR.y()) * 8;
}

void CPU::ccf() {
  F.set(Flags::Z, false);
  F.set(Flags::H, false);
  F.set(Flags::C, !F.get(Flags::C));
}

void CPU::scf() {
  F.set(Flags::Z, false);
  F.set(Flags::H, false);
  F.set(Flags::C, true);
}

void CPU::daa() {
  uint8_t result = 0;
  uint8_t adjustment = 0;

  if (F.get(Flags::N)) {
    if (F.get(Flags::H)) {
      adjustment += 0x6;
    }

    if (F.get(Flags::C)) {
      adjustment += 0x60;
    }

    result = A - adjustment;
  } else {
    if (F.get(Flags::H) || (A & 0xF) > 0x9) {
      adjustment += 0x6;
    }

    if (F.get(Flags::C) || A > 0x99) {
      adjustment += 0x60;
      F.set(Flags::C, true);
    }

    result = A + adjustment;
  }

  F.set(Flags::Z, result == 0);
  F.set(Flags::H, false);

  A = result;
}

void CPU::prefix() {
  IR = Opcode(fetch_byte());
  const auto cb_ins = cb_optable[IR.val];

  (this->*cb_ins.exec)();

#ifndef NDEBUG
  log_ins(cb_ins);
#endif
}

void CPU::illegal() {
  std::cerr << "Illegal opcode encountered: 0x" << std::hex
            << static_cast<int>(IR.val) << "\n";
  state = State::Stopped;
}
