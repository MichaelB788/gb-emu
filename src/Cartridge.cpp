#include "Cartridge.hpp"
#include "Common.hpp"
#include <array>
#include <filesystem>
#include <fstream>

namespace {
void validate_rom_path(const std::filesystem::path &rom_path) {
  if (rom_path.empty())
    throw std::invalid_argument("Path to ROM was not given.");

  else if (!std::filesystem::exists(rom_path))
    throw std::invalid_argument(std::string("Could not find ROM in path: ") +
                                rom_path.string());

  else if (rom_path.extension() != ".gb")
    throw std::invalid_argument(std::string("Given file is incompatible: ") +
                                rom_path.string());
}
} // namespace

Cartridge::Cartridge(const std::filesystem::path &rom_path) {
  static constexpr std::array<size_t, 6> ram_sizes{
      {0, KIB_2, KIB_8, KIB_32, KIB_128, KIB_64}};

  validate_rom_path(rom_path);

  std::ifstream rom_file{rom_path, std::ios::binary};

  if (!rom_file.is_open())
    throw std::runtime_error("Failed to open the ROM file.");

  rom.resize(std::filesystem::file_size(rom_path));
  rom_file.read(reinterpret_cast<char *>(rom.data()), rom.size());

  ext_ram.resize(ram_sizes[rom[0x149]]);
}
