# Thornhill

## Prerequisites

- Node.js (for the hot reload toolchain, currently not working)
- **(Microsoft Windows only):** you will need to install [Windows Subsystem for Linux 2 (WSL 2)](https://learn.microsoft.com/en-us/windows/wsl/install). Then, all commands should be executed from the WSL environment (easily accessed by opening a Command Prompt / PowerShell / Windows Terminal window and typing `wsl` at the prompt).
  

## Setup
These instructions are for **Ubuntu Linux** (assuming you have superuser/administrator privileges) and/or **Windows with an up-to-date version of WSL 2**.
- For **macOS on Intel**, follow these instructions but you'll need to use `brew` to install dependencies.
- For **macOS on Apple Silicon**, refer to [Building on M1 Mac](https://github.com/SamJakob/thornhill/wiki/Building-on-M1-Mac)
- For **older versions of Windows and WSL**, refer to [Building on Windows (Older Versions)](https://github.com/SamJakob/thornhill/wiki/Building-on-Windows-(Older-Versions))
- For **Ubuntu Linux *without superuser/administrator privileges***, see: [Building on Ubuntu Linux (Unprivileged User)](https://github.com/SamJakob/thornhill/wiki/Building-on-Ubuntu-Linux-(Unprivileged-User)).

### **Step 1: Ensure the necessary compilers/dependencies are installed.**

```bash
# Install compilers and build tools.
# Note that CMake >= 3.20 is required but at this point,
# that should be the default for any package manager.
sudo apt install gcc g++ build-essential cmake
```

### **Step 2: Install build dependencies**

```bash
# Install build dependencies
# (qemu-system isn't a build dep, obviously, but it's sure useful to test with.)
sudo apt update 
sudo apt install gnu-efi mtools qemu-system

# Install hot-reload dependencies (skip for now)
# cd scripts/hotreload
# yarn                  # (to install dependencies)
# cd ../..
```

### **Step 3: Build the system**

```bash
# Build the system
mkdir build
cd build

cmake ../
make clean

make emulator # or just `make` to build without starting emulator
```

### **Step 4: Start the hot-reload toolchain**

The hot-reload toolchain is currently unavailable. (Not yet refactored to account for switch to cmake.)  
It's some simple tweaks (switching out the command and checking it all works - perhaps optimizing the buildchain a little), I'm just focusing on other tasks within the OS at the moment.


## Commands

- `make clean`: Cleans the build and creates the initial directory structure, intended for preparation of an initial build.
- `make`: Builds the system.
- `make emulator`: Runs QEMU with the built-in QEMU EFI firmware and the system image.
- Hot-reload toolchain is currently unavailable. (Not yet refactored to account for switch to cmake.)
