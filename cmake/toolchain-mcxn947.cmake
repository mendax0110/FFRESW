# ==============================================================================
# NXP FRDM-MCXN947 CMake Toolchain File
# ==============================================================================
# Usage:
#   cmake .. -DPLATFORM=MCXN947 -DCMAKE_TOOLCHAIN_FILE=../cmake/toolchain-mcxn947.cmake
# ==============================================================================

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

# Toolchain prefix
set(ARM_TOOLCHAIN_PREFIX "arm-none-eabi-" CACHE STRING "ARM toolchain prefix")

find_program(CMAKE_C_COMPILER   ${ARM_TOOLCHAIN_PREFIX}gcc     REQUIRED)
find_program(CMAKE_CXX_COMPILER ${ARM_TOOLCHAIN_PREFIX}g++     REQUIRED)
find_program(CMAKE_ASM_COMPILER ${ARM_TOOLCHAIN_PREFIX}gcc     REQUIRED)
find_program(CMAKE_OBJCOPY      ${ARM_TOOLCHAIN_PREFIX}objcopy REQUIRED)
find_program(CMAKE_OBJDUMP      ${ARM_TOOLCHAIN_PREFIX}objdump REQUIRED)
find_program(CMAKE_SIZE         ${ARM_TOOLCHAIN_PREFIX}size    REQUIRED)

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
