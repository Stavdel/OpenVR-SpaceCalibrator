cmake_minimum_required(VERSION 3.10)
project(OpenVR-SpaceCalibrator)

# Build libraries
add_library(gl3w STATIC ../lib/gl3w/src/gl3w.c)
target_include_directories(gl3w PRIVATE ../lib/gl3w/include)

add_library(imgui STATIC 
    ../lib/imgui/imgui.cpp 
    ../lib/imgui/imgui_draw.cpp 
    ../lib/imgui/imgui_demo.cpp
    ../lib/imgui/imgui_impl_glfw.cpp
    ../lib/imgui/imgui_impl_opengl3.cpp
)
target_include_directories(imgui PRIVATE ../lib/gl3w/include ../lib/imgui)

# Find required packages
find_package(Eigen3 REQUIRED NO_MODULE)
find_package(PkgConfig REQUIRED)
pkg_check_modules(GLFW REQUIRED glfw3)
pkg_check_modules(OPENVR REQUIRED openvr)

# Main executable
add_executable(openvr-spacecalibrator 
    Calibration.cpp 
    CalibrationCalc.cpp 
    Configuration.cpp 
    EmbeddedFiles.cpp 
    IPCClient.cpp
    OpenVR-SpaceCalibrator.cpp 
    UserInterface.cpp
)

target_include_directories(openvr-spacecalibrator PRIVATE 
    ../lib/gl3w/include
    ../lib/openvr/
    ../lib/imgui
    ${OPENVR_INCLUDE_DIRS}
)

target_link_libraries(openvr-spacecalibrator 
    Eigen3::Eigen
    gl3w
    imgui
    ${GLFW_LIBRARIES}
    ${OPENVR_LIBRARIES}
    ${CMAKE_DL_LIBS}
)



# Installation
install(TARGETS openvr-spacecalibrator DESTINATION bin)

# Install desktop entry and icon
if(INSTALL_DESKTOP)
    # Configure desktop file
    configure_file(
        "${CMAKE_SOURCE_DIR}/openvrspacecalibrator.desktop.in"
        "${CMAKE_BINARY_DIR}/openvrspacecalibrator.desktop"
        @ONLY
    )
    
    install(FILES "${CMAKE_BINARY_DIR}/openvrspacecalibrator.desktop"
            DESTINATION share/applications)
    
    install(FILES icon.png 
            DESTINATION share/icons
            RENAME OpenVR-SpaceCalibrator.png)
endif()
