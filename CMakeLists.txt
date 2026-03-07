# ==============================================================================
# FFRESW – Top-level CMake Build System
# ==============================================================================
# Supports two target platforms:
#   -DPLATFORM=AVR        → Arduino ATmega2560  (avr-gcc toolchain)
#   -DPLATFORM=MCXN947    → NXP FRDM-MCXN947   (arm-none-eabi-gcc toolchain)
#
# Usage examples:
#   mkdir build-avr && cd build-avr
#   cmake .. -DPLATFORM=AVR -DCMAKE_TOOLCHAIN_FILE=../cmake/toolchain-avr.cmake
#   cmake --build .
#
#   mkdir build-mcxn && cd build-mcxn
#   cmake .. -DPLATFORM=MCXN947 -DCMAKE_TOOLCHAIN_FILE=../cmake/toolchain-mcxn947.cmake
#   cmake --build .
# ==============================================================================

cmake_minimum_required(VERSION 3.20)
project(FFRESW
    VERSION 0.3.0
    LANGUAGES C CXX ASM
    DESCRIPTION "Farnsworth Fusion Reactor Embedded Software"
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# ------------------------------------------------------------------------------
#  Platform selection
# ------------------------------------------------------------------------------
if(NOT DEFINED PLATFORM)
    message(FATAL_ERROR "PLATFORM is not set. Use -DPLATFORM=AVR or -DPLATFORM=MCXN947")
endif()

message(STATUS "Building for platform: ${PLATFORM}")

if(PLATFORM STREQUAL "AVR")
    add_compile_definitions(PLATFORM_AVR=1 ARDUINO=10813 __AVR_ATmega2560__=1)
elseif(PLATFORM STREQUAL "MCXN947")
    add_compile_definitions(PLATFORM_MCXN947=1 CPU_MCXN947VDF=1 FRDM_MCXN947=1)
else()
    message(FATAL_ERROR "Unknown PLATFORM '${PLATFORM}'. Use AVR or MCXN947.")
endif()

# ------------------------------------------------------------------------------
#  Source directories
# ------------------------------------------------------------------------------
set(LIB_DIR  ${CMAKE_SOURCE_DIR}/eSW/libraries)
set(MAIN_DIR ${CMAKE_SOURCE_DIR}/eSW/FFRESW/FFRESW)

# Collect all library source files
set(LIB_SOURCES
    ${LIB_DIR}/hal/hal.h
    ${LIB_DIR}/hal/platform.h
    ${LIB_DIR}/calcModule/calcModule.cpp
    ${LIB_DIR}/comModule/comModule.cpp
    ${LIB_DIR}/comModule/ETHH.cpp
    ${LIB_DIR}/comModule/I2CC.cpp
    ${LIB_DIR}/comModule/SER.cpp
    ${LIB_DIR}/comModule/SPII.cpp
    ${LIB_DIR}/flyback/flyback.cpp
    ${LIB_DIR}/jsonModule/jsonModule.cpp
    ${LIB_DIR}/logManager/logManager.cpp
    ${LIB_DIR}/pressure/pressure.cpp
    ${LIB_DIR}/reportSystem/reportSystem.cpp
    ${LIB_DIR}/sensorModule/sensorModule.cpp
    ${LIB_DIR}/serialMenu/serialMenu.cpp
    ${LIB_DIR}/temperature/temperature.cpp
    ${LIB_DIR}/timeModule/timeModule.cpp
    ${LIB_DIR}/vacControl/vacControl.cpp
)

# Platform-specific HAL source
if(PLATFORM STREQUAL "AVR")
    list(APPEND LIB_SOURCES ${LIB_DIR}/hal/hal_avr.cpp)
elseif(PLATFORM STREQUAL "MCXN947")
    list(APPEND LIB_SOURCES ${LIB_DIR}/hal/hal_mcxn947.cpp)
endif()

# Collect all include directories
set(LIB_INCLUDE_DIRS
    ${LIB_DIR}/hal
    ${LIB_DIR}/calcModule
    ${LIB_DIR}/comModule
    ${LIB_DIR}/config
    ${LIB_DIR}/flyback
    ${LIB_DIR}/jsonModule
    ${LIB_DIR}/lockGuard
    ${LIB_DIR}/logManager
    ${LIB_DIR}/pressure
    ${LIB_DIR}/ptrUtils
    ${LIB_DIR}/reportSystem
    ${LIB_DIR}/sensorModule
    ${LIB_DIR}/serialMenu
    ${LIB_DIR}/taskWatchDog
    ${LIB_DIR}/temperature
    ${LIB_DIR}/timeModule
    ${LIB_DIR}/vacControl
)

# ------------------------------------------------------------------------------
#  Library target
# ------------------------------------------------------------------------------
add_library(ffresw_lib STATIC ${LIB_SOURCES})
target_include_directories(ffresw_lib PUBLIC ${LIB_INCLUDE_DIRS})

# ------------------------------------------------------------------------------
#  Main firmware target
# ------------------------------------------------------------------------------
# For Arduino the .ino is treated as .cpp
set(MAIN_SOURCE ${MAIN_DIR}/FFRESW.ino)
set_source_files_properties(${MAIN_SOURCE} PROPERTIES LANGUAGE CXX)

add_executable(ffresw_firmware ${MAIN_SOURCE})
target_link_libraries(ffresw_firmware PRIVATE ffresw_lib)
target_include_directories(ffresw_firmware PRIVATE ${LIB_INCLUDE_DIRS})

# ------------------------------------------------------------------------------
#  Platform-specific post-build (hex generation, flashing, etc.)
# ------------------------------------------------------------------------------
if(PLATFORM STREQUAL "AVR")
    # Generate .hex for AVR flashing
    add_custom_command(TARGET ffresw_firmware POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -O ihex -R .eeprom
                $<TARGET_FILE:ffresw_firmware>
                $<TARGET_FILE_DIR:ffresw_firmware>/ffresw_firmware.hex
        COMMENT "Generating HEX file for AVR"
    )

    # Optional: flash target using avrdude
    add_custom_target(flash
        COMMAND avrdude -c wiring -p atmega2560 -P /dev/ttyACM0 -b 115200
                -D -U flash:w:$<TARGET_FILE_DIR:ffresw_firmware>/ffresw_firmware.hex:i
        DEPENDS ffresw_firmware
        COMMENT "Flashing firmware to ATmega2560"
    )

elseif(PLATFORM STREQUAL "MCXN947")
    # Generate .bin for ARM flashing
    add_custom_command(TARGET ffresw_firmware POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -O binary
                $<TARGET_FILE:ffresw_firmware>
                $<TARGET_FILE_DIR:ffresw_firmware>/ffresw_firmware.bin
        COMMENT "Generating BIN file for MCXN947"
    )

    # Size report
    add_custom_command(TARGET ffresw_firmware POST_BUILD
        COMMAND ${CMAKE_SIZE} $<TARGET_FILE:ffresw_firmware>
        COMMENT "Firmware size report"
    )

    # Optional: flash target using LinkServer / J-Link
    add_custom_target(flash
        COMMAND LinkServer flash MCXN947:FRDM-MCXN947
                load $<TARGET_FILE_DIR:ffresw_firmware>/ffresw_firmware.bin
        DEPENDS ffresw_firmware
        COMMENT "Flashing firmware to FRDM-MCXN947"
    )
endif()

# ------------------------------------------------------------------------------
#  Doxygen documentation target
# ------------------------------------------------------------------------------
find_package(Doxygen QUIET)
if(DOXYGEN_FOUND)
    add_custom_target(docs
        COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_SOURCE_DIR}/docs/doxygen/Doxyfile
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Generating Doxygen documentation"
    )
endif()
