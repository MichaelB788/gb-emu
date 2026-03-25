#ifndef GAMEBOY_CONSTANTS_HPP
#define GAMEBOY_CONSTANTS_HPP
#include <cstddef>
#include <cstdint>

// clang-format off
static constexpr size_t KIB_128 = 131072,
                        KIB_64 = 65536,
                        KIB_32 = 32768,
                        KIB_16 = 16384,
                        KIB_8 = 8192,
                        KIB_4 = 4096,
                        KIB_2 = 2048;

static constexpr uint16_t ROM_BANK_0_ADDR = 0x0000,
                          ROM_BANK_N_ADDR = 0x4000,
                          VRAM_ADDR = 0x8000,
                          EXT_RAM_ADDR = 0xA000,
                          WRAM_BANK_0_ADDR = 0xC000,
                          WRAM_BANK_N_ADDR = 0xD000,
                          ECHO_RAM_ADDR = 0xE000,
                          OAM_ADDR = 0xFE00,
                          IO_REG_ADDR = 0xFF00,
                          HRAM_ADDR = 0xFF80,
                          IE_ADDR = 0xFFFF;

#endif
