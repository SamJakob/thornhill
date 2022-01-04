# Enable Assembly.
enable_language(ASM)

# Set source & include directories.
set(CMAKE_SOURCE_DIR ../)
include_directories(../)
file(GLOB_RECURSE THK_SOURCES *.cpp *.c ../drivers/*.cpp ../drivers/*.c ../lib/*.cpp ../lib/*.c ../font/*.cpp ../font/*.c)

# Set up compiler and linker flags.
set(CMAKE_C_FLAGS "-mcmodel=large -ffreestanding --target=x86_64-none-elf")
set(CMAKE_CXX_FLAGS "-mcmodel=large -fPIC --target=x86_64-none-elf -ffreestanding -I. -Iarch/ -Iarch/${TH_ARCH} -O3 -Wall -Wextra -Wpedantic -std=c++20 -nostdlib -fstack-protector-all -mno-red-zone -fPIC")
set(CMAKE_CXX_LINK_FLAGS "-nostdlib -fPIC -Wl,--gc-sections -mno-red-zone -T linker.ld")
set(CMAKE_CXX_LINK_EXECUTABLE "${CMAKE_CXX_LINK_EXECUTABLE_NAME} <CMAKE_CXX_LINK_FLAGS> -o <TARGET> <OBJECTS> <LINK_LIBRARIES>")
configure_file(linker.ld linker.ld COPYONLY)

# Load architecture-specific libraries.
add_library(gdt_loader.o OBJECT "arch/${TH_ARCH}/gdt/gdt_loader.s")
add_library(interrupt_handler.o OBJECT "arch/${TH_ARCH}/interrupt/interrupt_handler.s")

# Produce kernel executable.
add_executable(kernel "${THK_SOURCES}")
add_dependencies(kernel bootloader)
target_link_libraries(kernel gdt_loader.o interrupt_handler.o)

# Add standard library
set_property(
        TARGET kernel
        APPEND PROPERTY
            INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/../lib/std/include"
        APPEND PROPERTY
            INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/../lib/thornhill/include"
)

# Copy kernel executable to output directory.
add_custom_command(TARGET kernel POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy kernel ${TH_OUTPUT_DIRECTORY}/kernel
        )

# Finally add kernel to bootable image
add_custom_target(populate-${TH_OUTPUT_IMG}
        COMMENT "Adding kernel to bootable image..."
        COMMAND ${CMAKE_COMMAND} -E copy ${TH_OUTPUT_DIRECTORY}/${TH_OUTPUT_IMG} thornhill.img
        COMMAND "${TH_BUILD_SCRIPTS_DIRECTORY}/populateimg.sh"
        COMMAND ${CMAKE_COMMAND} -E copy thornhill.img ${TH_OUTPUT_DIRECTORY}/${TH_OUTPUT_IMG}
        BYPRODUCTS ${TH_OUTPUT_IMG}
        )
add_dependencies(populate-${TH_OUTPUT_IMG} kernel)

# Add a pseudo-target to build the kernel and populate the bootloader image with the kernel.
add_custom_target(thornhill)
add_dependencies(thornhill populate-${TH_OUTPUT_IMG})
