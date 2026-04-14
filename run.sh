#!/usr/bin/env bash

set -e

PROJECT_ROOT=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)
BUILD_DIR="${PROJECT_ROOT}/build"
BUILD_TYPE="${BUILD_TYPE:-Debug}"

clean=false
extra_args=()
for arg in "$@"; do
  case "$arg" in
  --clean) clean=true ;;
  --) ;;
  *) extra_args+=("$arg") ;;
  esac
done

if [[ $clean ]]; then
  rm -rf "$BUILD_DIR"
fi

cmake -S "$PROJECT_ROOT" -B "$BUILD_DIR" \
  -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
  -DCMAKE_BUILD_TYPE="$BUILD_TYPE" \
  -G Ninja
ninja -C "$BUILD_DIR"

exe="${BUILD_DIR}/bin/GameBoy"
rom="${PROJECT_ROOT}/extern/gb-test-roms/cpu_instrs/cpu_instrs.gb"

[[ -x "$exe" ]] || {
  echo "Could not find exe" >&2
  exit 1
}

"$exe" "$rom"
