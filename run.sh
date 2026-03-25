#!/bin/bash

set -euo pipefail

run() {
  if ! command -v cmake >/dev/null; then
    echo "Could not find cmake binary. Please install it via your package manager."
    exit 1
  elif ! command -v ninja >/dev/null; then
    echo "Could not find ninja binary. Please install it via your package manager."
    exit 1
  fi

  local project_root
  project_root=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)
  local build_dir="${project_root}/build"

  local cmake_opts=(
    "-DCMAKE_EXPORT_COMPILE_COMMANDS=ON"
    "-DCMAKE_BUILD_TYPE=Debug"
  )

  cmake -S "${project_root}" -B "${build_dir}" "${cmake_opts[@]}" -G Ninja
  ninja -C "${build_dir}"

  local exe="${build_dir}/bin/GameBoy"
  local rom="${project_root}/extern/gb-test-roms/cpu_instrs/cpu_instrs.gb"

  if ! [ -f "$rom" ]; then
    echo "ROM not found: $rom"
    exit 1
  fi

  "$exe" "$rom"
}

run
