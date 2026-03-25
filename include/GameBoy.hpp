#ifndef GAMEBOY_HPP
#define GAMEBOY_HPP
#include "CPU.hpp"
#include "Cartridge.hpp"
#include <filesystem>
#include <string>

class GameBoy {
public:
  explicit GameBoy(const std::filesystem::path &rom)
      : cart_(rom), bus_(cart_), cpu_(bus_) {}

  void run();

private:
  Cartridge cart_;

  Bus bus_;

  CPU cpu_;

  std::string msg;
};

#endif // !GAMEBOY_HPP
