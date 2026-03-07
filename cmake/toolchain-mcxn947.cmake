# ==============================================================================
# NXP FRDM-MCXN947 CMake Toolchain File
# ==============================================================================
# Usage:
#   cmake .. -DPLATFORM=MCXN947 -DCMAKE_TOOLCHAIN_FILE=../cmake/toolchain-mcxn947.cmake
#
# Prerequisites – install the ARM GNU toolchain:
#   Ubuntu/Debian:  sudo apt install gcc-arm-none-eabi
#   macOS (brew):   brew install --cask gcc-arm-embedded
#   Manual:         https://developer.arm.com/downloads/-/gnu-rm
#   Then ensure arm-none-eabi-gcc is on your PATH.
# ==============================================================================

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

# Toolchain prefix – override with -DARM_TOOLCHAIN_PREFIX=<prefix> if installed
# in a non-standard location.
set(ARM_TOOLCHAIN_PREFIX "arm-none-eabi-" CACHE STRING "ARM toolchain prefix")

find_program(CMAKE_C_COMPILER   ${ARM_TOOLCHAIN_PREFIX}gcc)
find_program(CMAKE_CXX_COMPILER ${ARM_TOOLCHAIN_PREFIX}g++)
find_program(CMAKE_ASM_COMPILER ${ARM_TOOLCHAIN_PREFIX}gcc)
find_program(CMAKE_OBJCOPY      ${ARM_TOOLCHAIN_PREFIX}objcopy)
find_program(CMAKE_OBJDUMP      ${ARM_TOOLCHAIN_PREFIX}objdump)
find_program(CMAKE_SIZE         ${ARM_TOOLCHAIN_PREFIX}size)

if(NOT CMAKE_C_COMPILER OR NOT CMAKE_CXX_COMPILER)
    message(FATAL_ERROR
        "\n"
        "==========================================================\n"
        " arm-none-eabi-gcc NOT FOUND\n"
        "==========================================================\n"
        " Install the ARM GNU toolchain first:\n"
        "   Ubuntu/Debian : sudo apt install gcc-arm-none-eabi\n"
        "   macOS (brew)  : brew install --cask gcc-arm-embedded\n"
        "   Manual        : https://developer.arm.com/downloads/-/gnu-rm\n"
        "\n"
        " Or pass a custom prefix:\n"
        "   -DARM_TOOLCHAIN_PREFIX=/path/to/arm-none-eabi-\n"
        "==========================================================\n"
    )
endif()

# Cortex-M33 flags for MCXN947 (dual-core Cortex-M33)
set(CPU_FLAGS "-mcpu=cortex-m33 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16")

set(CMAKE_C_FLAGS_INIT   "${CPU_FLAGS} -Os -Wall -Wextra -ffunction-sections -fdata-sections -fno-common")
set(CMAKE_CXX_FLAGS_INIT "${CPU_FLAGS} -Os -Wall -Wextra -ffunction-sections -fdata-sections -fno-common -fno-exceptions -fno-rtti -fno-threadsafe-statics")
set(CMAKE_ASM_FLAGS_INIT "${CPU_FLAGS}")

set(CMAKE_EXE_LINKER_FLAGS_INIT "${CPU_FLAGS} -Wl,--gc-sections -Wl,-Map=output.map --specs=nano.specs --specs=nosys.specs -T${CMAKE_SOURCE_DIR}/cmake/MCXN947_flash.ld")

# Disable compiler tests (cross-compiling)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# Search paths
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
