# EFI Library Options
set(THB_EFI_ROOT /usr/include/efi)
set(THB_EFI_INCLUDES "-I${THB_EFI_ROOT} -I${THB_EFI_ROOT}/${TH_ARCH} -I${THB_EFI_ROOT}/protocol")
set(THB_EFI_LIB_DIR /usr/lib)
set(THB_EFI_CRT_OBJECTS "${THB_EFI_LIB_DIR}/crt0-efi-${TH_ARCH}.o")
set(THB_EFI_LINKER_SCRIPT "${THB_EFI_LIB_DIR}/elf_${TH_ARCH}_efi.lds")

# Load sources and set output directory.
file(GLOB THB_SOURCES *.c)

# Set compiler flags.
set(CMAKE_C_FLAGS "${THB_EFI_INCLUDES} -ffreestanding -fno-stack-protector -c -fpic -fshort-wchar -mno-red-zone")
if (${TH_ARCH} MATCHES "x86_64")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DHAVE_USE_MS_ABI")
endif ()

# Set linker flags and command.
set(CMAKE_C_LINK_EXECUTABLE "/usr/bin/ld <OBJECTS> <CMAKE_C_LINK_FLAGS> -o <TARGET> <LINK_LIBRARIES>")
set(CMAKE_C_LINK_FLAGS "-nostdlib -T ${THB_EFI_LINKER_SCRIPT} -shared -Bsymbolic -L ${THB_EFI_LIB_DIR} ${THB_EFI_CRT_OBJECTS} -lefi -lgnuefi")

# Build executable from sources.
add_executable(BOOTX64.so ${THB_SOURCES})

# Now copy into an EFI App object.
file(GENERATE OUTPUT "GenerateEFIApp-$<CONFIG>.sh"
    CONTENT "${OBJCOPY} -j .text -j .sdata -j .data -j .dynamic -j .dynsym -j .rel -j .rela -j .reloc --target=efi-app-${TH_ARCH} BOOTX64.so BOOTX64.EFI"
    FILE_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE
)
add_custom_target(BOOTX64.EFI
    COMMAND "./GenerateEFIApp-$<CONFIG>.sh"
    COMMENT "Building ${TH_ARCH} EFI app from linked executable..."
    BYPRODUCTS BOOTX64.EFI
)
add_dependencies(BOOTX64.EFI BOOTX64.so)

# Finally, make a bootable image.
add_custom_target(${TH_OUTPUT_IMG}
    COMMAND "${TH_BUILD_SCRIPTS_DIRECTORY}/mkimg.sh"
    COMMENT "Building bootable image..."
    BYPRODUCTS ${TH_OUTPUT_IMG}
)
add_dependencies(${TH_OUTPUT_IMG} BOOTX64.EFI)
add_custom_command(TARGET ${TH_OUTPUT_IMG} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy thornhill.img ${TH_OUTPUT_DIRECTORY}/${TH_OUTPUT_IMG}
)

# Add a pseudo-target to build the bootloader and bootloader image.
add_custom_target(bootloader)
add_dependencies(bootloader ${TH_OUTPUT_IMG})