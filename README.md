# Thornhill

## Prerequisites

- Python 3.11+ (only needed for the hot reload toolchain)
- **(Microsoft Windows only):** you will need to install [Windows Subsystem for Linux 2 (WSL 2)](https://learn.microsoft.com/en-us/windows/wsl/install). Then, all commands should be executed from the WSL environment (easily accessed by opening a Command Prompt / PowerShell / Windows Terminal window and typing `wsl` at the prompt).
  

## Setup
These instructions are for **Ubuntu Linux** (assuming you have superuser/administrator privileges) and/or **Windows with an up-to-date version of WSL 2**. If you're running under a different distribution of Linux, you should be able to use these instructions with some minor modifications for your distro. (*I'll leave that to you - I'm sure you know what you're doing*).

- For **macOS on Intel**, follow these instructions but you'll need to use `brew` to install dependencies.
- For **macOS on Apple Silicon**, refer to [Building on M1 Mac](https://github.com/SamJakob/thornhill/wiki/Building-on-M1-Mac)
- For **older versions of Windows and WSL**, refer to [Building on Windows (Older Versions)](https://github.com/SamJakob/thornhill/wiki/Building-on-Windows-(Older-Versions))
- For **Ubuntu Linux *without superuser/administrator privileges***, see: [Building on Ubuntu Linux (Unprivileged User)](https://github.com/SamJakob/thornhill/wiki/Building-on-Ubuntu-Linux-(Unprivileged-User)) (*As above, for non-Ubuntu distributions of Linux, you should be able to use these instructions with minor modifications.*).

### **Step 0: Clone the repository**
The repository has third-party modules under `third-party/`. These are submodules and are not fetched by default. To clone them as well, you'll need `--recurse-submodules`.
```bash
git clone -c core.autocrlf=false --recurse-submodules git@github.com:SamJakob/thornhill.git
```

**On Windows**, note that if autocrlf is enabled (as is the default), this will cause problems for all Linux scripts.
You can turn off autocrlf for just this repository (and prevent it from running on clone) by setting `-c core.autocrlf=false` when you clone.

Ideally, under Windows you'll want to clone the repository into the Windows file-system and then drop into WSL by calling `bash` from that directory.
* **When building the toolchain under Windows (with WSL)** make sure that you use a temporary directory (which is the default if `toolchain/toolchain-build` does not exist -- so delete it if it does)
otherwise filesystem operations will be *painfully slow*.

### **Step 1: Ensure the necessary compilers/dependencies are installed.**
(!) Use of the correct cross-compiler is now enforced and MUST be used. Using system compilers often causes errors and
even if the code compiles successfully it will likely result in a broken kernel image for reasons that are not
immediately obvious.

The build system will automatically detect if the toolchain in `toolchain/` has been built and installed to the default
location (`toolchain/prefix`) and will automatically use it if it was. Otherwise, you will need to add the
cross-compiler to your PATH (so that find_program can find it). If you don't want to do this you will need to make
modifications.

- You'll need the x86_64-elf cross-compiler and binutils. These can either be built from the `toolchain/`
directory using `toolchain/build-toolchain.sh` OR some systems/distributions such as macOS or Arch Linux
(via AUR) might provide `x86_64-elf-gcc` and `x86_64-elf-binutils`.

- Additionally, CMake >= 3.20 is required, though at this point that should be the default version installed
from any package manager.

```bash
./toolchain/build-toolchain.sh
```

If you would prefer to do this manually, please refer to [Building the toolchain manually](https://github.com/SamJakob/thornhill/wiki/Building-on-Ubuntu-Linux-(Unprivileged-User)#building-the-toolchain-manually).

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

Install the dependencies for the hot-reload toolchain:
```bash
# Requires Python 3.11+
python3 --version

# Install pip dependencies
# (run from the root of the project)
python3 -m pip install -r ./scripts/hotreload/requirements.txt
```

The hot-reload toolchain is currently unavailable. (Not yet refactored to account for switch to cmake.)  
It's some simple tweaks (switching out the command and checking it all works - perhaps optimizing the buildchain a little), I'm just focusing on other tasks within the OS at the moment.


## Commands

- `make clean`: Cleans the build and creates the initial directory structure, intended for preparation of an initial build.
- `make`: Builds the system.
- `make emulator`: Runs QEMU with the built-in QEMU EFI firmware and the system image.
- `make hot`: Compiles Thornhill, boots the emulator and starts the hot-reload toolchain.
