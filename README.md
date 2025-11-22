# OpenVR Space Calibrator - Linux with Continuous Calibration

A Linux port of OpenVR Space Calibrator with full continuous calibration support. This tool aligns multiple tracking systems by bridging the gap between your headset's tracking space and your external trackers (e.g., mixing a standalone headset with Lighthouse trackers).

**Key Features:**
- **Continuous Calibration:** Smooth, real-time adjustments that refine over time.
- **Compatibility:** Works with ALVR, SteamVR, Lighthouse tracking, and mixed VR setups.

For Monado/WiVRn users, see [MoToC](https://github.com/galister/motoc) instead.

## Building

The following packages are required to build:
* `cmake`
* `glfw`
* `eigen`
* `openvr`

**Arch Linux:**
```bash
pacman -S cmake glfw-x11 eigen openvr
```

**Debian 11 / Ubuntu 22.10:**
```bash
sudo apt install cmake libeigen3-dev libglfw3-dev libopenvr-dev
```

### Quick Install (Recommended)
```bash
git clone https://github.com/Stavdel/OpenVR-SpaceCalibrator.git
cd OpenVR-SpaceCalibrator
./build-and-install.sh
```

### Uninstall
To remove OpenVR Space Calibrator:
```bash
cd OpenVR-SpaceCalibrator
./uninstall.sh
```

### Manual Build
```bash
git clone https://github.com/Stavdel/OpenVR-SpaceCalibrator.git
cd OpenVR-SpaceCalibrator
mkdir build && cd build
cmake .. -DCMAKE_INSTALL_PREFIX=$HOME/.local
make -j$(nproc)
make install
```

### Build Options
You can customize the build with these CMake options:
```bash
cmake .. -DINSTALL_DRIVER=OFF           # Skip driver installation
cmake .. -DINSTALL_DESKTOP=OFF          # Skip desktop entry
cmake .. -DSTEAMVR_DIR=/custom/path     # Custom SteamVR directory
```

## Running

The driver will be loaded automatically by SteamVR after installation. However, the **companion software (UI)** must be started separately to perform the calibration.

1. Launch SteamVR.
2. Start the `openvr-spacecalibrator` companion software:
   - From your desktop menu (if installed with desktop entry)
   - Or run directly: `~/.local/bin/openvr-spacecalibrator`

## Calibration Guide

### 1. Initial Calibration
You must perform an initial calibration when setting up for the first time, if you move your base stations, or if your playspace drifts significantly.

1. **Prepare Devices:** Ensure your headset and at least one tracked device (e.g., a Vive Tracker or Controller) are powered on and tracking.
2. **Open Space Calibrator:** Open the UI on your desktop or inside the SteamVR dashboard.
3. **Select Spaces:**
   - **Reference Space (Left)
   - **Target Space (Right)
   *Tip: If you aren't sure which device is which, click "Identify selected devices" to make the LED blink or the controller vibrate.*
4. **Calibrate:**
   - Hold the target device firmly against the reference device. Move your in a figure eight like pattern to collect data samples.
   - Click **Start Calibration**.
   - **Calibration Speed:** Use `Slow` or `Very Slow` for wireless headsets to ensure accuracy.
   - Walk around your play area, rotating the devices to capture different angles.
5. **Finish:** Once the progress bar completes, your devices will snap into the correct position.

### 2. Continuous Calibration
Once a base calibration is established, you can enable Continuous Calibration to keep devices aligned automatically during gameplay.

1. **Prepare Devices:** Ensure your headset and at least one tracked device (e.g., a Vive Tracker or Controller) are powered on and tracking.
2. **Open Space Calibrator:** Open the UI on your desktop or inside the SteamVR dashboard.
3. **Select Spaces:**
   - **Reference Space (Left)
   - **Target Space (Right)
   *Tip: If you aren't sure which device is which, click "Identify selected devices" to make the LED blink or the controller vibrate.*
4. Check **"Start Continuous Calibration"** in the Space Calibrator UI.
5. Play normally. The system will:
   - Smoothly interpolate positions (no snapping).
   - Automatically refine accuracy as you move around the room.
     

### 3. Copying Chaperone Bounds (Optional)
If you want your SteamVR Chaperone to match your headset's native Guardian/Boundary:
1. Ensure your headset's native boundary is set up correctly.
2. Open Space Calibrator in the SteamVR dashboard.
3. Click `Copy Chaperone Bounds to profile`.


## Troubleshooting

*   **Base stations appear in the wrong place:** This is visual only and can usually be ignored if the trackers align correctly with the headset.
*   **Trackers not showing in SteamVR:** Restart SteamVR. Ensure all dongles are firmly connected.
*   **"Red Mountains" in performance graphs / Drifting:** This usually indicates a sync issue between the two tracking systems. Restart SteamVR, ensure the headset is connected *before* turning on lighthouses, and re-run calibration.
*   **Disconnected Dongles:** If a USB dongle disconnects, SteamVR usually requires a full restart to recognize it again.

## Device Compatibility Notes

This tool aligns multiple tracking systems. Results vary depending on your hardware:

- **Wireless Headsets + Lighthouse Trackers:** Works well for stationary or moderate movement (Beat Saber, VRChat). Extensive room-scale movement may require Continuous Calibration to correct drift.
- **WMR / SLAM HMDs + Lighthouse:** Excellent for limited movement; may experience slight drift over large areas.
- **Oculus CV1 + Vive Devices:** Generally works very well with high stability.

## Uninstalling

To completely remove OpenVR Space Calibrator from your system:

1. Navigate to the source directory:
   ```bash
   cd OpenVR-SpaceCalibrator
   ```

2. Run the uninstall script:
   ```bash
   ./uninstall.sh
   ```

The uninstall script will:
- Remove the companion software executable
- Remove the desktop entry and icon
- Remove the OpenVR driver from SteamVR
- Clean up empty directories

**Note:** You may need to restart SteamVR after uninstalling for the driver to be fully removed.

## Credits

- Original code by [pushrax](https://github.com/pushrax)
- Base Linux fork by [gyroninja](https://github.com/gyroninja)
- Hook logic from [AngriestSCV](https://github.com/AngriestSCV)'s fork
- Additional features from [bd_](https://github.com/bdunderscore)'s fork
- Continuous Calibration Linux port and fixes by [Stavdel](https://github.com/Stavdel)
