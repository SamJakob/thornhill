# Enable Assembly.
set(CMAKE_ASM_LINK_EXECUTABLE "/usr/bin/as")
enable_language(ASM)

# Set source & include directories.
set(CMAKE_SOURCE_DIR ../)
include_directories(../)
file(GLOB_RECURSE THK_SOURCES *.cpp *.c)

# Set up compiler and linker flags.
set(CMAKE_CXX_LINK_EXECUTABLE "/usr/bin/g++ <CMAKE_CXX_LINK_FLAGS> <OBJECTS> -o <TARGET> <LINK_LIBRARIES>")
set(CMAKE_CXX_FLAGS "-ffreestanding -n -Wl,--gc-sections -I. -Iarch/ -Iarch/${TH_ARCH} -O2 -Wall -Wextra -nostdlib -mgeneral-regs-only -mno-red-zone")
set(CMAKE_CXX_LINK_FLAGS "-nostdlib -Wl,--gc-sections -mgeneral-regs-only -mno-red-zone -T linker.ld")
configure_file(linker.ld linker.ld COPYONLY)

# Load architecture-specific libraries.
add_library(gdt_loader.o OBJECT "arch/${TH_ARCH}/gdt/gdt_loader.s")
add_library(interrupt_handler.o OBJECT "arch/${TH_ARCH}/interrupt/interrupt_handler.s")

# Produce kernel executable.
add_executable(kernel main.cpp)
add_dependencies(kernel bootloader)
target_link_libraries(kernel gdt_loader.o)
target_link_libraries(kernel interrupt_handler.o)

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