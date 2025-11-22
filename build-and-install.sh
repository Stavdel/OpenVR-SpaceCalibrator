#!/usr/bin/env bash
set -e

SRC_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
BUILD_DIR="${BUILD_DIR:-$SRC_DIR/build}"

# Create build directory
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Configure with CMake
cmake "$SRC_DIR" \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX="$HOME/.local" \
    -DSTEAMVR_DIR="$HOME/.local/share/Steam/steamapps/common/SteamVR" \
    "$@"

# Build
make -j$(nproc)

# Install
make install

echo "OpenVR Space Calibrator has been built and installed!"
echo "Start SteamVR, then launch the companion software from your desktop menu or run:"
echo "$HOME/.local/bin/openvr-spacecalibrator"