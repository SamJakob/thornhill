# =========================================================================== #

#   /$$$$$$$$ /$$                                     /$$       /$$ /$$ /$$   #
#  |__  $$__/| $$                                    | $$      |__/| $$| $$   #
#     | $$   | $$$$$$$   /$$$$$$   /$$$$$$  /$$$$$$$ | $$$$$$$  /$$| $$| $$   #
#     | $$   | $$__  $$ /$$__  $$ /$$__  $$| $$__  $$| $$__  $$| $$| $$| $$   #
#     | $$   | $$  \ $$| $$  \ $$| $$  \__/| $$  \ $$| $$  \ $$| $$| $$| $$   #
#     | $$   | $$  | $$| $$  | $$| $$      | $$  | $$| $$  | $$| $$| $$| $$   #
#     | $$   | $$  | $$|  $$$$$$/| $$      | $$  | $$| $$  | $$| $$| $$| $$   #
#     |__/   |__/  |__/ \______/ |__/      |__/  |__/|__/  |__/|__/|__/|__/   #
#                                                                             #
#                                                                             #

# =========================================================================== #

#
# Notes
#

# The barebones-toolchain used in this project can be found here:
# https://github.com/rm-hull/barebones-toolchain



# =========================================================================== #

#
# General Options
#
ARCH				= x86_64

#
# Bootloader Options
#
EFI_ROOT			= /usr/include/efi
EFI_INCS			= -I$(EFI_ROOT) -I$(EFI_ROOT)/$(ARCH) -I$(EFI_ROOT)/protocol
EFI_LIB				= /usr/lib

EFI_CRT_OBJS		= $(EFI_LIB)/crt0-efi-$(ARCH).o
EFI_LDS				= $(EFI_LIB)/elf_$(ARCH)_efi.lds

BOOT_CC				= gcc
BOOT_LD				= ld

BOOT_CFLAGS			= $(EFI_INCS) -ffreestanding -fno-stack-protector -c -fpic -fshort-wchar -mno-red-zone
BOOT_LDFLAGS		= -nostdlib -T $(EFI_LDS) -shared -Bsymbolic -L $(EFI_LIB) $(EFI_CRT_OBJS) -lefi -lgnuefi

ifeq ($(ARCH),x86_64)
	BOOT_CFLAGS += -DHAVE_USE_MS_ABI
endif

#
# System Options
#
# CC	 			= ~/barebones-toolchain/cross/x86_64/bin/i686-elf-gcc
# CXX 				= ~/barebones-toolchain/cross/x86_64/bin/i686-elf-g++
GDB		 			= gdb

ELF_FORMAT			= elf64

C_SOURCES 			= $(wildcard kernel/*.cpp drivers/*.cpp cpu/${ARCH}/*.cpp)
HEADERS				= $(wildcard kernel/*.h drivers/*.h)

#OBJ					= ${HEADERS}
#${C_SOURCES:.cpp=.o}

# (-g = use debugging symbols in GDB)
CFLAGS				= -g

# =========================================================================== #

#
# Output Filepaths
#
KERNEL			::= ./out/system/kernel
BOOT_IMAGE 		::= ./out/bootstrap/boot.img

# =========================================================================== #

#
# Makefile
#

# If no parameters are supplied, simply run.
all: run

.PHONY: bootstrap ./out/bootstrap/main.o ./out/bootstrap/data.o ./out/bootstrap/BOOTX64.so ./out/bootstrap/BOOTX64.EFI ${BOOT_IMAGE}
.PHONY: kernel ./out/system/kern_thornhill.o ${KERNEL}

#
# Bootloader
#
./out/bootstrap/main.o: ./boot/main.c
	${BOOT_CC} ${BOOT_CFLAGS} -o $@ $<

./out/bootstrap/data.o: ./boot/data.c
	${BOOT_CC} ${BOOT_CFLAGS} -o $@ $<

./out/bootstrap/BOOTX64.so: ./out/bootstrap/main.o ./out/bootstrap/data.o
	${BOOT_LD} $^ ${BOOT_LDFLAGS} -o $@

./out/bootstrap/BOOTX64.EFI: ./out/bootstrap/BOOTX64.so
	objcopy -j .text -j .sdata -j .data -j .dynamic -j .dynsym -j .rel -j .rela -j .reloc --target=efi-app-$(ARCH) $^ $@

${BOOT_IMAGE}:
	make ./out/bootstrap/BOOTX64.EFI

	# Make FAT image.
	dd if=/dev/zero of=${BOOT_IMAGE} bs=512 count=90000
	mkfs.fat -v -F 32 -S 512 -s 1 ${BOOT_IMAGE} -n THORNHILL
	mmd -i ${BOOT_IMAGE} ::/EFI
	mmd -i ${BOOT_IMAGE} ::/EFI/BOOT
	mcopy -i ${BOOT_IMAGE} ./out/bootstrap/BOOTX64.EFI ::/EFI/BOOT


#
# CPU
#
CPU_ARCH = ./cpu/${ARCH}

./out/system/memory/gdt_loader.o: ./kernel/memory/gdt_loader.s
	as $< -o $@
OBJ += ./out/system/memory/gdt_loader.o


#
# Kernel
#
./out/system/kern_thornhill.o: ./kernel/main.cpp
	${CXX} -ffreestanding -n -Wl,--gc-sections -I${CPU_ARCH} -T linker.ld -c $< -o $@ -O2 -Wall -Wextra -nostdlib -mgeneral-regs-only -mno-red-zone

${KERNEL}: ./out/system/kern_thornhill.o ${OBJ}
	${CXX} $^ -nostdlib -Wl,--gc-sections -mgeneral-regs-only -mno-red-zone -o $@
	mcopy -i ${BOOT_IMAGE} $@ ::/



#
# Targets
#

bootstrap:
	make ${BOOT_IMAGE}

kernel:
	make ${KERNEL}

thornhill:
	make bootstrap
	make kernel

run:
	qemu-system-x86_64 -bios /usr/share/qemu/OVMF.fd ./out/bootstrap/boot.img

#debug: ./out/thornhill.raw ./out/kern_thornhill.elf
#	qemu-system-x86_64 -s -S -fda ./out/thornhill.raw &
#	${GDB} -ex "target remote localhost:1234" -ex "symbol-file ./out/kern_thornhill.elf"

clean:
	rm -rf *.bin *.dis *.o thornhill.raw *.elf
	rm -rf kernel/*.o boot/*.bin drivers/*.o boot/*.o

	rm -rf ./out
	make _prepareOutputStructure

#
# Helpers
#
_prepareOutputStructure:
	mkdir ./out
	mkdir ./out/bootstrap
	mkdir ./out/cpu
	mkdir ./out/system
	mkdir ./out/system/memory

#
# Generic Wildcard Rules
#

%.o: %.cpp ${HEADERS}
	${CXX} ${CFLAGS} -ffreestanding -c $< -o $@

#%.o: %.c ${HEADERS}
#	${CXX} ${CFLAGS} -ffreestanding -c $< -o $@