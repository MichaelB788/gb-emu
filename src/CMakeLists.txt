set(bin_name GameBoy)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_library(GameBoySource
        Bus.cpp
        Cartridge.cpp
        CMakeLists.txt
        CPU.cpp
        GameBoy.cpp
        PlatformSDL.cpp
)

add_executable(${bin_name} main.cpp)

find_package(SDL3 REQUIRED CONFIG COMPONENTS REQUIRED SDL3-shared)

target_link_libraries(${bin_name}
        SDL3::SDL3
        GameBoySource
)

target_compile_options(${bin_name} PRIVATE
        -Wall
        -Wextra
        -Wpedantic
        -Wconversion
        -Wsign-conversion
        -Wnarrowing
)
