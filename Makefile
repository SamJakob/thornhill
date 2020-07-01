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
CXX 		= ~/barebones-toolchain/cross/x86_64/bin/i686-elf-g++
LD 			= ~/barebones-toolchain/cross/x86_64/bin/i686-elf-ld
GDB 		= gdb

C_SOURCES 	= $(wildcard kernel/*.cpp drivers/*.cpp)
HEADERS		= $(wildcard kernel/*.h drivers/*.h)

OBJ			= ${C_SOURCES:.cpp=.o}

# (-g = use debugging symbols in GDB)
CFLAGS		= -g

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
	nasm $< -f elf -o $@

./out/kern_thornhill.o: ./kernel/main.cpp
	${CXX} -ffreestanding -c $< -o $@ -O2 -Wall -Wextra -fno-exceptions -fno-rtti

# --oformat binary
./out/kern_thornhill.bin: ./out/kern_thornhill_entry.o ./out/kern_thornhill.o
	${CXX} -o $@ -T linker.ld $^ -Wl,--oformat=binary -ffreestanding -O2 -nostdlib -lgcc -shared -g

./out/thornhill.bin: ./out/boot_thornhill.bin ./out/kern_thornhill.bin
	cat $^ > $@

# (for debugging purposes)
./out/kern_thornhill.elf: ./out/kern_thornhill_entry.o ${OBJ}
	${CXX} -o $@ -T linker.ld $^ -ffreestanding -O2 -nostdlib -lgcc -shared -g


#
# Targets
#

thornhill:
	make ./out/thornhill.bin

run: ./out/thornhill.bin
	qemu-system-x86_64 -fda $<

debug: ./out/thornhill.bin ./out/kern_thornhill.elf
	qemu-system-x86_64 -s -S -fda ./out/thornhill.bin &
	${GDB} -ex "target remote localhost:1234" -ex "symbol-file ./out/kern_thornhill.elf"

clean:
	rm -rf *.bin *.dis *.o thornhill.bin *.elf
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
		nasm $< -f elf -o $@

%.bin: %.asm
		nasm $< -f bin -o $@