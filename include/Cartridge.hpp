#ifndef GAMEBOY_CARTRIDGE_HPP
#define GAMEBOY_CARTRIDGE_HPP
#include "Common.hpp"
#include <cstdint>
#include <filesystem>
#include <vector>

class Cartridge {
public:
  explicit Cartridge(const std::filesystem::path &rom_path);

  [[nodiscard]] uint8_t read_rom_bank_0(const uint16_t address) const {
    return rom[address];
  }

  [[nodiscard]] uint8_t read_rom_bank_n(const uint16_t address) const {
    return rom[static_cast<size_t>((curr_rom_bank * KIB_16) +
                                   (address - ROM_BANK_N_ADDR))];
  }

  [[nodiscard]] uint8_t read_ext_ram(const uint16_t address) const {
    if (ext_ram.size() > 0)
      return ext_ram[static_cast<size_t>((curr_ext_ram_bank * KIB_8) +
                                         (address - EXT_RAM_ADDR))];
    else
      return 0xFF;
  }

  void set_rom_bank(const uint8_t bank) { curr_rom_bank = bank; }

  void set_ext_ram_bank(const uint8_t bank) { curr_ext_ram_bank = bank; }

  // TODO: MBC implementation
  // clang-format off
  void write_rom_bank_0(const uint16_t address, const uint8_t val) {
    (void)address; (void)val;
  }

  void write_rom_bank_n(const uint16_t address, const uint8_t val) {
    (void)address; (void)val;
  }

  void write_ext_ram_bank_n(const uint16_t address, const uint8_t val) {
    (void)address; (void)val;
  }
  // clang-format on

private:
  uint8_t curr_rom_bank{1};

  uint8_t curr_ext_ram_bank{0};

  std::vector<uint8_t> rom{};

  std::vector<uint8_t> ext_ram{};
};

#endif // !GAMEBOY_CARTRIDGE_HPP
