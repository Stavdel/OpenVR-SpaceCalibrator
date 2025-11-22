cmake_minimum_required(VERSION 3.10)
project(OpenVR-SpaceCalibratorDriver)

# Driver library
add_library(openvr-spacecalibratordriver SHARED 
    Hooking.cpp 
    InterfaceHookInjector.cpp 
    IPCServer.cpp 
    Logging.cpp
    OpenVR-SpaceCalibratorDriver.cpp 
    ServerTrackedDeviceProvider.cpp
)

target_include_directories(openvr-spacecalibratordriver PRIVATE ../lib/openvr/)
target_link_libraries(openvr-spacecalibratordriver ${CMAKE_DL_LIBS})

# Set output name to match SteamVR expectations
set_target_properties(openvr-spacecalibratordriver PROPERTIES
    OUTPUT_NAME "driver_01spacecalibrator"
    PREFIX ""
)

# Install driver files
if(INSTALL_DRIVER AND STEAMVR_DIR)
    # Install driver manifest
    install(FILES 01spacecalibrator/driver.vrdrivermanifest
            DESTINATION "${STEAMVR_DIR}/drivers/01spacecalibrator")
    
    # Install driver resources
    install(DIRECTORY 01spacecalibrator/resources
            DESTINATION "${STEAMVR_DIR}/drivers/01spacecalibrator")
    
    # Install driver binary
    install(TARGETS openvr-spacecalibratordriver
            DESTINATION "${STEAMVR_DIR}/drivers/01spacecalibrator/bin/linux64")
    
    # Custom install script for driver registration
    install(CODE "
        message(STATUS \"Registering OpenVR driver with SteamVR...\")
        find_program(VRPATHREG vrpathreg.sh PATHS \"${STEAMVR_DIR}/bin\")
        if(VRPATHREG)
            execute_process(
                COMMAND \"\${VRPATHREG}\" removedriver \"${STEAMVR_DIR}/drivers/01spacecalibrator\"
                OUTPUT_QUIET ERROR_QUIET
            )
            execute_process(
                COMMAND \"\${VRPATHREG}\" adddriver \"${STEAMVR_DIR}/drivers/01spacecalibrator\"
                OUTPUT_QUIET ERROR_QUIET
            )
            message(STATUS \"Driver registered successfully\")
        else()
            message(WARNING \"vrpathreg.sh not found, driver not registered automatically\")
        endif()
    " COMPONENT Runtime)
endif()