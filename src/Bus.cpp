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
    return wram_[addr - WRAM_BANK_0_ADDR];

  if (ECHO_RAM_ADDR <= addr && addr < OAM_ADDR)
    return wram_[addr - ECHO_RAM_ADDR];

  if (IO_REG_ADDR <= addr && addr < HRAM_ADDR)
    return io_.read(addr);

  if (HRAM_ADDR <= addr && addr < IE_ADDR)
    return hram_[addr - HRAM_ADDR];

  if (addr == IE_ADDR)
    return IE;

  return 0x10;
}

void Bus::write(const uint16_t addr, const uint8_t val) {
  if (0x0000 <= addr && addr <= 0x3FFF)
    cart_.write_rom_bank_0(addr, val);

  else if (0x4000 <= addr && addr <= 0x7FFF)
    cart_.write_rom_bank_n(addr, val);

  else if (0x8000 <= addr && addr <= 0x9FFF)
    ppu_.write_vram(addr, val);

  else if (0xA000 <= addr && addr <= 0xBFFF)
    cart_.write_ext_ram_bank_n(addr, val);

  else if (0xC000 <= addr && addr <= 0xCFFF)
    wram_[addr - 0xC000] = val;

  else if (0xD000 <= addr && addr <= 0xDFFF)
    wram_[addr - 0xC000] = val;

  else if (0xE000 <= addr && addr <= 0xFDFF)
    wram_[addr - 0xE000] = val;

  else if (0xFE00 <= addr && addr <= 0xFE9F)
    (void)val;

  else if (0xFEA0 <= addr && addr <= 0xFEFF)
    (void)val;

  else if (0xFF00 <= addr && addr <= 0xFF7F)
    io_.write(addr, val);

  else if (0xFF80 <= addr && addr <= 0xFFFE)
    hram_[addr - HRAM_ADDR] = val;

  else if (addr == IE_ADDR)
    IE = val;
}
