# Enable Assembly.
enable_language(ASM)

# Set source & include directories.
set(CMAKE_SOURCE_DIR ../)
include_directories(../)
file(GLOB_RECURSE THK_SOURCES *.cpp *.c ../drivers/*.cpp ../drivers/*.c ../lib/*.cpp ../lib/*.c ../font/*.cpp ../font/*.c)

# Locate relevant crt*.o files.
execute_process(
        COMMAND ${CMAKE_CXX_COMPILER} -print-file-name=crtbegin.o
        OUTPUT_VARIABLE CRT_BEGIN_OBJ
        OUTPUT_STRIP_TRAILING_WHITESPACE
)
execute_process(
        COMMAND ${CMAKE_CXX_COMPILER} -print-file-name=crtend.o
        OUTPUT_VARIABLE CRT_END_OBJ
        OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Set up compiler and linker flags.
set(CMAKE_C_FLAGS "-mcmodel=large -ffreestanding")
set(CMAKE_CXX_FLAGS "-mcmodel=large -ffreestanding -n -Wl,--gc-sections -I. -Iarch/ -Iarch/${TH_ARCH} -O3 -Wall -Wextra -nostdlib -mgeneral-regs-only -mno-red-zone -fstack-protector-all -fPIC")
set(CMAKE_CXX_LINK_FLAGS "-nostdlib -lgcc -Wl,--gc-sections -mgeneral-regs-only -mno-red-zone -T linker.ld -fPIC")
set(CMAKE_CXX_LINK_EXECUTABLE "${CMAKE_CXX_LINK_EXECUTABLE_NAME} <CMAKE_CXX_LINK_FLAGS> <OBJECTS> -o <TARGET> crti.o ${CRT_BEGIN_OBJ} <LINK_LIBRARIES> ${CRT_END_OBJ} crtn.o")
configure_file(linker.ld linker.ld COPYONLY)
configure_file(arch/${TH_ARCH}/abi/crti.o crti.o COPYONLY)
configure_file(arch/${TH_ARCH}/abi/crtn.o crtn.o COPYONLY)

# Load architecture-specific libraries.
add_library(gdt_loader.o OBJECT "arch/${TH_ARCH}/gdt/gdt_loader.s")
add_library(interrupt_handler.o OBJECT "arch/${TH_ARCH}/interrupt/interrupt_handler.s")

# Produce kernel executable.
add_executable(kernel "${THK_SOURCES}")
add_dependencies(kernel bootloader)
target_link_libraries(kernel gdt_loader.o interrupt_handler.o)

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