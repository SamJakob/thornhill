#!/usr/bin/env bash

# Exit on command failure.
set -e

# Check for dependencies
which makeinfo > /dev/null || (echo "You are missing 'makeinfo'. On Ubuntu you can install with 'sudo apt install texinfo'."; exit 1)

# Directories to build GCC and BINUTILS in.
GCCDIR="x86_64-elf-gcc"
BINDIR="x86_64-elf-binutils"

# Push current directory so we can return to it later.
pushd "$(pwd)"

# Begin build script
echo "-- Moving to toolchain directory"
cd "$(dirname "$0")"
[[ ! -d "./prefix" ]] && mkdir "./prefix"
INSTALL_PREFIX="$(pwd)/prefix"

echo "-- Enter a directory for toolchain installation prefix."
echo "   Or, leave blank for the default."
echo "   "
echo "   Default: $INSTALL_PREFIX"
read -p 'Toolchain Install Prefix: ' INSTALL_PREFIX_NEW
[[ ! -z "$INSTALL_PREFIX_NEW" ]] && INSTALL_PREFIX="$INSTALL_PREFIX_NEW"
[[ ! -d "$INSTALL_PREFIX" ]] && (echo "Invalid directory (missing directory) for installation prefix: $INSTALL_PREFIX - directory must exist but be empty."; exit 1) || (rm -rf "$INSTALL_PREFIX" && mkdir "$INSTALL_PREFIX")
# [ "$(ls -A $INSTALL_PREFIX)" ] && (echo "Invalid directory (not empty) for installation prefix: $INSTALL_PREFIX - directory must exist but be empty."; exit 1)

read -r -p "-- Are you sure you want to install to $INSTALL_PREFIX? [y/N] " confirmation
case "$confirmation" in
	[yY][eE][sS]|[yY])
		echo "-- OK. Continuing..."
		;;
	*)
		echo "-- User interrupted..."
		exit 1
		;;
esac

[[ "$(uname -a)" == *"WSL"* ]] && (
	echo -e "\033[0;31m[WARNING] You are building under WSL, you"
	echo -e "probably want to use a temporary directory for this"
	echo -e "as file system performance under WSL is horrible for"
	echo -e "the Windows file system.\033[0m"
)

echo "-- Choosing toolchain build directory"
echo "   (create ./toolchain-build to use in-repo directory"
echo "    otherwise, temporary directory will be used.)"
BUILD_DIR=./toolchain-build

if [[ ! -d "$BUILD_DIR" || "$(uname -a)" == *"WSL"* ]]; then
	BUILD_DIR="$(mktemp -d)";
	echo "-- Created temporary directory: $BUILD_DIR"
fi

BUILD_DIR="$(realpath "$BUILD_DIR")"
cd "$BUILD_DIR"

[[ "$(uname -a)" == *"WSL"* && "$BUILD_DIR" == "/mnt/" ]] && (
	echo -e "\033[0;31m[WARNING] You are building under WSL yet it seems you"
	echo -e "have selected a directory in the Windows file system."
	echo -e "This will likely have ABYSMAL performance. It's not too"
	echo -e "late to cancel and choose a temporary directory for this!!!\033[0m"
)

confirmation=""
read -r -p "-- Are you sure you want to build in $BUILD_DIR? [y/N] " confirmation
case "$confirmation" in
	[yY][eE][sS]|[yY])
		echo "-- OK. Continuing..."
		;;
	*)
		echo "-- User interrupted..."
		exit 1
		;;
esac

echo "-- Checking toolchain build directory ($BUILD_DIR)"
# [[ -d "./toolchain-build" ]] && rm -rf ./toolchain-build

echo "-- Checking/setting up directory structure"
if [[ ! -d "$BUILD_DIR/$GCCDIR" ]]; then
	mkdir "$BUILD_DIR/$GCCDIR"
	echo "-- Fetching and extracting gcc"
	wget https://ftp.gnu.org/gnu/gcc/gcc-12.2.0/gcc-12.2.0.tar.xz
	tar -C "$BUILD_DIR/$GCCDIR" -xf gcc-12.2.0.tar.xz
fi

if [[ ! -d "$BUILD_DIR/$BINDIR" ]]; then
	mkdir "$BUILD_DIR/$BINDIR"
	echo "-- Fetching and extracting binutils"
	wget https://ftp.gnu.org/gnu/binutils/binutils-2.40.tar.bz2
	tar -C "$BUILD_DIR/$BINDIR" -xf binutils-2.40.tar.bz2
fi

echo "-- Entering binutils build directory"
cd "$BUILD_DIR/$BINDIR/"
cd "$(ls -d ./*|head -n 1)"

echo "-- Configuring binutils"
./configure -C --target=x86_64-elf --enable-targets=all --disable-nls "--prefix=$INSTALL_PREFIX"

echo "-- Compiling and installing binutils"
make
make install

echo "-- Entering GCC build directory"
cd "$BUILD_DIR/$GCCDIR/"
cd "$(ls -d ./*|head -n 1)"

echo "-- Feching pre-requisites for GCC"
./contrib/download_prerequisites

echo "-- Entering GCC (temporary) build directory"
[[ ! -d "x86_64-elf-gcc-build" ]] && mkdir x86_64-elf-gcc-build
cd x86_64-elf-gcc-build

echo "-- Configuring GCC"
../configure -C --target=x86_64-elf "--prefix=$INSTALL_PREFIX" --disable-nls --without-isl --without-headers --enable-languages=c,c++ --disable-hosted-libstdcxx

echo "-- Compiling and installing GCC"
make all-gcc
make install-gcc
make all-target-libgcc
make install-target-libgcc

# End build script

# Start user's shell in current directory
# USER_SHELL="$(grep ^$USER: /etc/passwd | grep -Eo ':([^:]*)$' | tr -d ':')"
# echo ""
# echo "-- You can now install gcc (./$GCCDIR) and binutils (./$BINDIR)"
# echo "   to a desired location."
# echo "-- Starting your default shell ($USER_SHELL)"
# echo "-- Type 'exit' when done."
# $USER_SHELL

# Return to user's directory
echo "-- Returning to initial directory."
echo "-- All done!"
cd "$(popd)"
