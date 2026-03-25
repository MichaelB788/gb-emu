#include "GameBoy.hpp"
#include "PlatformSDL.hpp"
#include <filesystem>
#include <iostream>

int main(const int argc, const char **argv) {
  std::filesystem::path rom = argc > 1 ? argv[1] : "";

  try {
    PlatformSDL sdl{};
    GameBoy gb{rom};

    gb.run();
  } catch (const ExceptionSDL &sdl_e) {
    std::cerr << "SDL Exception occured: " << sdl_e.what() << std::endl;
  } catch (const std::exception &e) {
    std::cerr << "Exception occured: " << e.what() << std::endl;
  }
}
