#include "GameBoy.hpp"

void GameBoy::run() {
  while (cpu_.is_running()) {
    cpu_.step();
  }
}
