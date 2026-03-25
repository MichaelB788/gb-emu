#include "Bus.hpp"
#include "Cartridge.hpp"
#include "Common.hpp"
#include "PPU.hpp"

uint8_t Bus::read(const uint16_t addr) const {
  if (ROM_BANK_0_ADDR <= addr && addr < ROM_BANK_N_ADDR)
    return cart_.read_rom_bank_0(addr);

  if (ROM_BANK_N_ADDR <= addr && addr < VRAM_ADDR)
    return cart_.read_rom_bank_n(addr);

  if (VRAM_ADDR <= addr && addr < EXT_RAM_ADDR)
    return ppu_.read_vram(addr);

  if (EXT_RAM_ADDR <= addr && addr < WRAM_BANK_0_ADDR)
    return cart_.read_ext_ram(addr);

  if (WRAM_BANK_0_ADDR <= addr && addr < ECHO_RAM_ADDR)
    return read_wram_bank_0(addr);

  if (ECHO_RAM_ADDR <= addr && addr < OAM_ADDR)
    return read_echo_ram(addr);

  if (IO_REG_ADDR <= addr && addr < HRAM_ADDR)
    return io_.read(addr);

  if (HRAM_ADDR <= addr && addr < IE_ADDR)
    return read_hram(addr);

  if (addr == IE_ADDR)
    return IE;

  return 0x10;
}

void Bus::write(const uint16_t addr, const uint8_t val) {
  if (ROM_BANK_0_ADDR <= addr && addr < ROM_BANK_N_ADDR)
    cart_.write_rom_bank_0(addr, val);

  else if (ROM_BANK_N_ADDR <= addr && addr < VRAM_ADDR)
    cart_.write_rom_bank_n(addr, val);

  else if (VRAM_ADDR <= addr && addr < EXT_RAM_ADDR)
    ppu_.write_vram(addr, val);

  else if (EXT_RAM_ADDR <= addr && addr < WRAM_BANK_0_ADDR)
    cart_.write_ext_ram_bank_n(addr, val);

  else if (WRAM_BANK_0_ADDR <= addr && addr < ECHO_RAM_ADDR)
    write_wram_bank_0(addr, val);

  else if (ECHO_RAM_ADDR <= addr && addr < OAM_ADDR)
    write_echo_ram(addr, val);

  else if (IO_REG_ADDR <= addr && addr < HRAM_ADDR)
    io_.write(addr, val);

  else if (HRAM_ADDR <= addr && addr < IE_ADDR)
    write_hram(addr, val);

  else if (addr == IE_ADDR)
    IE = val;
}
