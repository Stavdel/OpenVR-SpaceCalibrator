cmake_minimum_required(VERSION 3.10)
project(OpenVR-SpaceCalibrator)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(PkgConfig REQUIRED)

# Default installation paths
if(NOT CMAKE_INSTALL_PREFIX)
    set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()

# SteamVR path detection
if(NOT STEAMVR_DIR)
    if(DEFINED ENV{HOME})
        set(STEAMVR_DIR "$ENV{HOME}/.local/share/Steam/steamapps/common/SteamVR" CACHE PATH "SteamVR installation directory")
    endif()
endif()

# Options
option(INSTALL_DRIVER "Install OpenVR driver to SteamVR" ON)
option(INSTALL_DESKTOP "Install desktop entry and icon" ON)

add_subdirectory(OpenVR-SpaceCalibrator)
add_subdirectory(OpenVR-SpaceCalibratorDriver)

# Custom install target
install(CODE "
    message(STATUS \"Installing OpenVR Space Calibrator...\")
" COMPONENT Runtime)

# Print configuration summary
message(STATUS "")
message(STATUS "OpenVR Space Calibrator Configuration:")
message(STATUS "  Install prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "  SteamVR directory: ${STEAMVR_DIR}")
message(STATUS "  Install driver: ${INSTALL_DRIVER}")
message(STATUS "  Install desktop: ${INSTALL_DESKTOP}")
message(STATUS "")