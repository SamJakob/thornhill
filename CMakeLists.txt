cmake_minimum_required(VERSION 3.20)

# Project details.
project(thornhill)

# Set build system configuration.
set(CMAKE_GENERATOR "Unix Makefiles")
set(CMAKE_EXPORT_COMPILE_COMMANDS true)

# Set project configuration.
set(TH_ARCH "x86_64")
set(CMAKE_BUILD_TYPE Debug)

# Set global build system filenames and locations.
get_filename_component(TH_BUILD_SCRIPTS_DIRECTORY   "./scripts" ABSOLUTE)
get_filename_component(TH_OUTPUT_DIRECTORY          "./out" ABSOLUTE)

# Set global build system properties.
set(TH_OUTPUT_IMG "thornhill.img")

# Use cross-compilers where necessary.
if ("${CMAKE_HOST_SYSTEM_NAME}" STREQUAL "Darwin")
    message("Non-native executable environment detected. Using cross-compilers.")
    set(CMAKE_ASM_LINK_EXECUTABLE x86_64-elf-as)
    set(CMAKE_C_COMPILER x86_64-elf-gcc)
    set(CMAKE_CXX_COMPILER x86_64-elf-g++)
    set(CMAKE_C_LINK_EXECUTABLE_NAME x86_64-elf-ld)
    set(CMAKE_CXX_LINK_EXECUTABLE_NAME x86_64-elf-g++)

    set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
    set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
    set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
endif()

if (NOT DEFINED CMAKE_ASM_LINK_EXECUTABLE)
    set(CMAKE_ASM_LINK_EXECUTABLE "/usr/bin/as")
endif()
if (NOT DEFINED CMAKE_C_LINK_EXECUTABLE_NAME)
    set(CMAKE_C_LINK_EXECUTABLE_NAME /usr/bin/ld)
endif ()
if (NOT DEFINED CMAKE_CXX_LINK_EXECUTABLE_NAME)
    set(CMAKE_CXX_LINK_EXECUTABLE_NAME /usr/bin/ld)
endif ()

# Check dependencies and prerequisites.
if (NOT EXISTS /usr/include/efi AND NOT EXISTS /usr/local/include/efi)
    message(FATAL_ERROR
        "You must install gnu-efi. \
        On macOS, this can be done by following the guide in the wiki.
        On Debian/Ubuntu, this can be done with `sudo apt install gnu-efi`."
    )
else()
    if (EXISTS /usr/include/efi)
        set(EFI_INCLUDE_ROOT /usr/include/efi)
        set(EFI_INCLUDE_LIB /usr/lib)
    elseif(EXISTS /usr/local/include/efi)
        set(EFI_INCLUDE_ROOT /usr/local/include/efi)
        set(EFI_INCLUDE_LIB /usr/local/lib)
    endif()
endif()

find_program(OBJCOPY NAMES "objcopy" "x86_64-elf-objcopy")
if (NOT OBJCOPY)
    message(FATAL_ERROR "Failed to find objcopy. If you're on Debian/Ubuntu, you might need to install binutils which"
            " can be done with `sudo apt install binutils` or `sudo apt install binutils-common` depending on your"
            " system version. On macOS, this can be done with `brew install x86_64-elf-binutils`.")
endif()

find_program(MKFSFAT NAMES "mkfs.fat")
if (NOT MKFSFAT)
    message(FATAL_ERROR
        "You must install dosfstools. \
        On macOS, this can be done with `brew install dosfstools`. \
        On Debian/Ubuntu, this can be done with `sudo apt install dosfstools`."
    )
endif()

find_program(MTOOLS NAMES "mtools")
if (NOT MTOOLS)
    message(FATAL_ERROR
        "You must install mtools. \
        On macOS, this can be done with `brew install mtools`. \
        On Debian/Ubuntu, this can be done with `sudo apt install mtools`."
    )
endif()

# Ensure output directory is ready.
execute_process(COMMAND "${CMAKE_COMMAND} -E make_directory ${TH_OUTPUT_DIRECTORY}")

# Run the version script.
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/config/version.h
        ${CMAKE_CURRENT_SOURCE_DIR}/config/_version.h
    COMMAND ${CMAKE_COMMAND} -DTH_CURRENT_SOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR}
        -P ${CMAKE_CURRENT_SOURCE_DIR}/version.cmake
)
add_custom_target(version ALL DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/config/version.h)

# Include components.
add_subdirectory(boot)
add_subdirectory(kernel)

# Add `emulator` and `run` pseudo-targets
add_custom_target(emulator)
add_dependencies(emulator thornhill)
add_custom_command(TARGET emulator
    COMMENT "Starting QEMU..."
    COMMAND ${TH_BUILD_SCRIPTS_DIRECTORY}/startemulator.sh "${TH_OUTPUT_DIRECTORY}/thornhill.img"
)
