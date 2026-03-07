# ==============================================================================
# AVR ATmega2560 CMake Toolchain File
# ==============================================================================
# Usage:
#   cmake .. -DPLATFORM=AVR -DCMAKE_TOOLCHAIN_FILE=../cmake/toolchain-avr.cmake
#
# Prerequisites – install the AVR toolchain:
#   Ubuntu/Debian:  sudo apt install gcc-avr avr-libc avrdude
#   macOS (brew):   brew install avr-gcc avrdude
#   Windows:        Install Arduino IDE or WinAVR, add to PATH.
# ==============================================================================

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR avr)

# Toolchain prefix – override with -DAVR_TOOLCHAIN_PREFIX=<prefix> if installed
# in a non-standard location.
set(AVR_TOOLCHAIN_PREFIX "avr-" CACHE STRING "AVR toolchain prefix")

find_program(CMAKE_C_COMPILER   ${AVR_TOOLCHAIN_PREFIX}gcc)
find_program(CMAKE_CXX_COMPILER ${AVR_TOOLCHAIN_PREFIX}g++)
find_program(CMAKE_ASM_COMPILER ${AVR_TOOLCHAIN_PREFIX}gcc)
find_program(CMAKE_OBJCOPY      ${AVR_TOOLCHAIN_PREFIX}objcopy)
find_program(CMAKE_OBJDUMP      ${AVR_TOOLCHAIN_PREFIX}objdump)
find_program(CMAKE_SIZE         ${AVR_TOOLCHAIN_PREFIX}size)

if(NOT CMAKE_C_COMPILER OR NOT CMAKE_CXX_COMPILER)
    message(FATAL_ERROR
        "\n"
        "==========================================================\n"
        " avr-gcc NOT FOUND\n"
        "==========================================================\n"
        " Install the AVR toolchain first:\n"
        "   Ubuntu/Debian : sudo apt install gcc-avr avr-libc\n"
        "   macOS (brew)  : brew install avr-gcc\n"
        "   Windows       : Install Arduino IDE or WinAVR\n"
        "\n"
        " Or pass a custom prefix:\n"
        "   -DAVR_TOOLCHAIN_PREFIX=/path/to/avr-\n"
        "==========================================================\n"
    )
endif()

# MCU-specific flags
set(AVR_MCU "atmega2560" CACHE STRING "AVR MCU type")
set(AVR_F_CPU "16000000UL" CACHE STRING "CPU frequency")

set(MCU_FLAGS "-mmcu=${AVR_MCU}")

set(CMAKE_C_FLAGS_INIT   "${MCU_FLAGS} -DF_CPU=${AVR_F_CPU} -Os -Wall -Wextra -ffunction-sections -fdata-sections")
set(CMAKE_CXX_FLAGS_INIT "${MCU_FLAGS} -DF_CPU=${AVR_F_CPU} -Os -Wall -Wextra -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-threadsafe-statics")
set(CMAKE_ASM_FLAGS_INIT "${MCU_FLAGS}")

set(CMAKE_EXE_LINKER_FLAGS_INIT "${MCU_FLAGS} -Wl,--gc-sections -Wl,-Map=output.map")

# Disable compiler tests (cross-compiling)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# Search paths
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
