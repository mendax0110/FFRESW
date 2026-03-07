# ==============================================================================
# AVR ATmega2560 CMake Toolchain File
# ==============================================================================
# Usage:
#   cmake .. -DPLATFORM=AVR -DCMAKE_TOOLCHAIN_FILE=../cmake/toolchain-avr.cmake
# ==============================================================================

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR avr)

# Toolchain prefix
set(AVR_TOOLCHAIN_PREFIX "avr-" CACHE STRING "AVR toolchain prefix")

find_program(CMAKE_C_COMPILER   ${AVR_TOOLCHAIN_PREFIX}gcc   REQUIRED)
find_program(CMAKE_CXX_COMPILER ${AVR_TOOLCHAIN_PREFIX}g++   REQUIRED)
find_program(CMAKE_ASM_COMPILER ${AVR_TOOLCHAIN_PREFIX}gcc   REQUIRED)
find_program(CMAKE_OBJCOPY      ${AVR_TOOLCHAIN_PREFIX}objcopy REQUIRED)
find_program(CMAKE_OBJDUMP      ${AVR_TOOLCHAIN_PREFIX}objdump REQUIRED)
find_program(CMAKE_SIZE         ${AVR_TOOLCHAIN_PREFIX}size    REQUIRED)

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
