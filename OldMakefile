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
GDB		 			= gdb

ELF_FORMAT			= elf64

C_SOURCES 			= $(wildcard kernel/*.cpp drivers/*.cpp arch/${ARCH}/*.cpp)
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
	dd if=/dev/zero of=/tmp/thornboot.img bs=512 count=90000
	mv /tmp/thornboot.img ${BOOT_IMAGE}
	mkfs.fat -v -F 32 -S 512 -s 1 ${BOOT_IMAGE} -n THORNHILL
	mmd -i ${BOOT_IMAGE} ::/EFI
	mmd -i ${BOOT_IMAGE} ::/EFI/BOOT
	mcopy -i ${BOOT_IMAGE} ./out/bootstrap/BOOTX64.EFI ::/EFI/BOOT


#
# CPU
#
CPU_ARCH = ./arch/${ARCH}

./out/system/${ARCH}/gdt_loader.o: ${CPU_ARCH}/gdt/gdt_loader.s
	as $< -o $@
OBJ += ./out/system/${ARCH}/gdt_loader.o

./out/system/${ARCH}/interrupt_handler.o: ${CPU_ARCH}/interrupt/interrupt_handler.s
	as $< -o $@
OBJ += ./out/system/${ARCH}/interrupt_handler.o


#
# Kernel
#
./out/system/kern_thornhill.o: ./kernel/main.cpp
	${CXX} -ffreestanding -n -Wl,--gc-sections -I. -I${CPU_ARCH} -T linker.ld -c $< -o $@ -O2 -Wall -Wextra -nostdlib -mgeneral-regs-only -mno-red-zone

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
	cp ./out/bootstrap/boot.img ./out/thornhill.img


emulator:
	qemu-system-x86_64 -qmp tcp:localhost:4444,server,nowait -bios /usr/share/qemu/OVMF.fd ./out/thornhill.img -monitor stdio

run:
	$(info Running preliminary build...)
	make bootstrap
	make kernel
	cp ./out/bootstrap/boot.img ./out/thornhill.img
	$(info Starting hot reload...)
	node build/hotreload

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
	mkdir ./out/system
	mkdir ./out/system/${ARCH}

#
# Generic Wildcard Rules
#

%.o: %.cpp ${HEADERS}
	${CXX} ${CFLAGS} -ffreestanding -c $< -o $@

#%.o: %.c ${HEADERS}
#	${CXX} ${CFLAGS} -ffreestanding -c $< -o $@