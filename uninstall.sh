#!/usr/bin/env bash
set -e

SRC_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
BUILD_DIR="${BUILD_DIR:-$SRC_DIR/build}"
INSTALL_PREFIX="${CMAKE_INSTALL_PREFIX:-$HOME/.local}"

# Default SteamVR path
STEAMVR_DIR="${STEAMVR_DIR:-$HOME/.local/share/Steam/steamapps/common/SteamVR}"

echo "Uninstalling OpenVR Space Calibrator..."

# Remove executable
rm -f "$INSTALL_PREFIX/bin/openvr-spacecalibrator"
    
# Remove desktop entry
rm -f "$INSTALL_PREFIX/share/applications/openvrspacecalibrator.desktop"
    
# Remove icon
rm -f "$INSTALL_PREFIX/share/icons/hicolor/256x256/apps/openvrspacecalibrator.png"
    
# Remove driver from SteamVR
if [ -d "$STEAMVR_DIR/drivers/01spacecalibrator" ]; then
    echo "Removing OpenVR driver from SteamVR..."
    rm -rf "$STEAMVR_DIR/drivers/01spacecalibrator"
fi

# Clean up empty directories
rmdir --ignore-fail-on-non-empty "$INSTALL_PREFIX/bin" 2>/dev/null || true
rmdir --ignore-fail-on-non-empty "$INSTALL_PREFIX/share/applications" 2>/dev/null || true
rmdir --ignore-fail-on-non-empty "$INSTALL_PREFIX/share/icons/hicolor/256x256/apps" 2>/dev/null || true
rmdir --ignore-fail-on-non-empty "$INSTALL_PREFIX/share/icons/hicolor/256x256" 2>/dev/null || true
rmdir --ignore-fail-on-non-empty "$INSTALL_PREFIX/share/icons/hicolor" 2>/dev/null || true
rmdir --ignore-fail-on-non-empty "$INSTALL_PREFIX/share/icons" 2>/dev/null || true
rmdir --ignore-fail-on-non-empty "$INSTALL_PREFIX/share" 2>/dev/null || true

echo "OpenVR Space Calibrator has been uninstalled!"
echo ""
echo "Note: You may need to restart SteamVR for the driver to be fully removed."