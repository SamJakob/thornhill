cmake_minimum_required(VERSION 3.20)

# Project details.
project(thornhill)

# Set project configuration.
set(TH_ARCH "x86_64")

# Set global build system filenames and locations.
get_filename_component(TH_BUILD_SCRIPTS_DIRECTORY   "./build" ABSOLUTE)
get_filename_component(TH_OUTPUT_DIRECTORY          "./out" ABSOLUTE)

# Set global build system properties.
set(TH_OUTPUT_IMG "thornhill.img")

# Check dependencies and prerequisites.
if (NOT EXISTS /usr/include/efi)
    message(FATAL_ERROR "You must install gnu-efi. On Debian/Ubuntu, this can be done with `sudo apt install gnu-efi`.")
endif()

find_program(OBJCOPY NAMES "objcopy")
if (NOT OBJCOPY)
    message(FATAL_ERROR "Failed to find objcopy. If you're on Debian/Ubuntu, you might need to install binutils which"
            " can be done with `sudo apt install binutils` or `sudo apt install binutils-common` depending on your"
            " system version.")
endif()

find_program(MKFSFAT NAMES "mkfs.fat")
if (NOT MKFSFAT)
    message(FATAL_ERROR "You must install dosfstools. On Debian/Ubuntu, this can be done with `sudo apt install dosfstools`.")
endif()

find_program(MTOOLS NAMES "mtools")
if (NOT MTOOLS)
    message(FATAL_ERROR "You must install mtools. On Debian/Ubuntu, this can be done with `sudo apt install mtools`.")
endif()

# Ensure output directory is ready.
execute_process(COMMAND "${CMAKE_COMMAND} -E make_directory ${TH_OUTPUT_DIRECTORY}")

# Include components.
add_subdirectory(boot_old)
add_subdirectory(kernel)

# Add `emulator` and `run` pseudo-targets
add_custom_target(emulator)
add_dependencies(emulator thornhill)
add_custom_command(TARGET emulator
    COMMENT "Starting QEMU..."
    COMMAND ${TH_BUILD_SCRIPTS_DIRECTORY}/startemulator.sh "${TH_OUTPUT_DIRECTORY}/thornhill.img"
)