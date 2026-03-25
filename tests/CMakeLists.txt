set(GAMEBOY_TESTS
  cpu-instrs-tests.cpp
)

find_package(Catch2 3 REQUIRED)

add_executable(cpu-tests ${GAMEBOY_TESTS})
target_link_libraries(cpu-tests PRIVATE
  Catch2::Catch2WithMain
)