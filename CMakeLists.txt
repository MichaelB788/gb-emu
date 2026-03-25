cmake_minimum_required(VERSION 3.20)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_EXTENSIONS OFF)

project(
        GameBoy
        VERSION 1.0
        DESCRIPTION "GameBoy emulator made with C++"
        LANGUAGES CXX
)

# Header files
include_directories(BEFORE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_BINARY_DIR}/include
)

add_subdirectory(src)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
  add_subdirectory(tests)
endif()
