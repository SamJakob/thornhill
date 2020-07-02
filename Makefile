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
# Options
#
CXX 			= ~/barebones-toolchain/cross/x86_64/bin/i686-elf-g++
LD 				= ~/barebones-toolchain/cross/x86_64/bin/i686-elf-ld
#CXX			= /usr/bin/g++
#LD				= /usr/bin/ld
GDB 			= gdb

ELF_FORMAT		= elf

C_SOURCES 		= $(wildcard kernel/*.cpp drivers/*.cpp)
HEADERS			= $(wildcard kernel/*.h drivers/*.h)

OBJ				= ${C_SOURCES:.cpp=.o}

# (-g = use debugging symbols in GDB)
CFLAGS			= -g
#LDFLAGS		= -Ttext 0x1000
LDFLAGS			= -T linker.ld

# =========================================================================== #

#
# Makefile
#

# If no parameters are supplied, simply run.
all: run

# For some reason without .PHONY, targets are not rebuilt and considered
# 'up-to-date'...
.PHONY: ./out/thornhill.bin ./out/kern_thornhill.bin ./out/kern_thornhill.o ./out/kern_thornhill_entry.o ./out/boot_thornhill.bin


#
# Dependencies
#

./out/boot_thornhill.bin: main.asm
	nasm -f bin $< -o $@

./out/kern_thornhill_entry.o: ./boot/kernel_entry.asm
	nasm $< -f ${ELF_FORMAT} -o $@

./out/kern_thornhill.o: ./kernel/main.cpp
	${CXX} -ffreestanding -c $< -o $@ -O2 -Wall -Wextra -fno-exceptions -fno-rtti

./out/kern_thornhill.bin: ./out/kern_thornhill_entry.o ./out/kern_thornhill.o
	${CXX} -o $@ ${LDFLAGS} $^ -Wl,--oformat=binary -ffreestanding -O2 -nostdlib -lgcc -shared -g

./out/thornhill.raw: ./out/boot_thornhill.bin ./out/kern_thornhill.bin
	cat $^ > $@

# (for debugging purposes)
./out/kern_thornhill.elf: ./out/kern_thornhill_entry.o ./out/kern_thornhill.o
	${CXX} -o $@ ${LDFLAGS} $^ -ffreestanding -O2 -nostdlib -lgcc -shared -g


#
# Targets
#

thornhill:
	make ./out/thornhill.raw

run: ./out/thornhill.raw
	qemu-system-x86_64 -fda $<

debug: ./out/thornhill.raw ./out/kern_thornhill.elf
	qemu-system-x86_64 -s -S -fda ./out/thornhill.raw &
	${GDB} -ex "target remote localhost:1234" -ex "symbol-file ./out/kern_thornhill.elf"

clean:
	rm -rf *.bin *.dis *.o thornhill.raw *.elf
	rm -rf kernel/*.o boot/*.bin drivers/*.o boot/*.o

	rm -rf ./out
	mkdir ./out

#
# Generic Wildcard Rules
#

%.o: %.cpp ${HEADERS}
		${CXX} ${CFLAGS} -ffreestanding -c $< -o $@

%.o: %.c ${HEADERS}
		${CXX} ${CFLAGS} -ffreestanding -c $< -o $@

%.o: %.asm
		nasm $< -f ${ELF_FORMAT} -o $@

%.bin: %.asm
		nasm $< -f bin -o $@