cmake_minimum_required(VERSION 3.20)

# Suppress compiler checks.
set(CMAKE_C_COMPILER_WORKS TRUE)
set(CMAKE_CXX_COMPILER_WORKS TRUE)
set(CMAKE_SYSTEM_NAME Generic CACHE STRING "" FORCE)

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

get_filename_component(TH_TOOLCHAIN_DIRECTORY		"./toolchain" ABSOLUTE)

# Set global build system properties.
set(TH_OUTPUT_IMG "thornhill.img")

# Check if the ./toolchain directory has been built.
# If all cross-compilers and tools were found, use them.
message("Looking for toolchain in ${TH_TOOLCHAIN_DIRECTORY}")
if(EXISTS "${TH_TOOLCHAIN_DIRECTORY}" AND EXISTS "${TH_TOOLCHAIN_DIRECTORY}/prefix/bin")
	# Attempt to use the toolchain prefix.
	message("Found built toolchain in ${TH_TOOLCHAIN_DIRECTORY}. Using cross-compilers.")
	set(CMAKE_ASM_LINK_EXECUTABLE "${TH_TOOLCHAIN_DIRECTORY}/prefix/bin/x86_64-elf-as")
	set(CMAKE_C_COMPILER "${TH_TOOLCHAIN_DIRECTORY}/prefix/bin/x86_64-elf-gcc")
	set(CMAKE_CXX_COMPILER "${TH_TOOLCHAIN_DIRECTORY}/prefix/bin/x86_64-elf-g++")
	set(CMAKE_C_LINK_EXECUTABLE_NAME "${TH_TOOLCHAIN_DIRECTORY}/prefix/bin/x86_64-elf-ld")
	set(CMAKE_CXX_LINK_EXECUTABLE_NAME "${TH_TOOLCHAIN_DIRECTORY}/prefix/bin/x86_64-elf-g++")
	set(OBJCOPY "${TH_TOOLCHAIN_DIRECTORY}/prefix/bin/x86_64-elf-objcopy")

	# Using cross-compiler, so only use host system paths and not sysroot.
	set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
	set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
	set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
else()
	# Otherwise, try to find cross-compilers installed elsewhere on the system.
	message("Did not locate a built toolchain in ${TH_TOOLCHAIN_DIRECTORY}. Checking elsewhere on system...")

	# Look for cross-compiler binutils elsewhere on the system (both as and ld).
	find_program(CROSS_ASM x86_64-elf-as)
	find_program(CROSS_LDD x86_64-elf-ld)
	# Look for cross-compiler GCC elsewhere on the system.
	find_program(CROSS_GCC x86_64-elf-gcc)

	if (CROSS_ASM AND CROSS_GCC AND CROSS_LDD)
		message("Valid cross-compiler environment detected on system. Using cross-compilers.")
		set(CMAKE_ASM_LINK_EXECUTABLE x86_64-elf-as)
		set(CMAKE_C_COMPILER x86_64-elf-gcc)
		set(CMAKE_CXX_COMPILER x86_64-elf-g++)
		set(CMAKE_C_LINK_EXECUTABLE_NAME x86_64-elf-ld)
		set(CMAKE_CXX_LINK_EXECUTABLE_NAME x86_64-elf-g++)

		# Using cross-compiler, so only use host system paths and not sysroot.
		set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
		set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
		set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
	else()
		message(FATAL_ERROR
			"Missing cross-compiler.\n"
			"You need freestanding versions of x86_64-elf-binutils and x86_64-elf-gcc.\n"
			"Please refer to the wiki for more information.")
	endif()
endif()

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
