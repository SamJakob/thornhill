# Thornhill

## Prerequisites

- Node.js
- **(Microsoft Windows only):** you will need to install [Windows Subsystem for Linux 2 (WSL 2)](https://docs.microsoft.com/en-us/windows/wsl/install-win10) **and** the [VcXsrv X server](https://sourceforge.net/projects/vcxsrv/files/latest/download) for Windows. After you’ve installed VcXsrv, double click the `wsl-vcxsrv-config.xlaunch` file in the root of the repository. All of these commands should be executed from the WSL environment (easily accessed by opening a Command Prompt / PowerShell / Windows Terminal window and typing `wsl` at the prompt).
  

## Setup

**Step 1: Ensure the necessary compilers/dependencies are installed.**

```bash
# Install compilers
sudo apt install gcc g++ buildessentials cmake

# NOTE: cmake must be version >= 3.20, if this is not yet available in your distro, you can build
# from source. (I recommend checking GitHub releases to ensure you're getting the latest version.)
sudo apt install libssl-dev
wget https://github.com/Kitware/CMake/releases/download/v3.20.2/cmake-3.20.2.tar.gz
tar -zxvf cmake-3.20.2.tar.gz
cd cmake-3.20.2
./bootstrap
make -j 8
sudo make install
```

**(Windows Only): Enable Display Output for WSL**

```bash
# Enable WSL display output to X server.
echo "export DISPLAY=$(cat /etc/resolv.conf | grep nameserver | awk '{print $2; exit;}'):0.0" >> ~/.bashrc
source ~/.bashrc
```

**Step 2: Install build dependencies**

```bash
# Install build dependencies
# (qemu-system isn't a build dep, obviously, but it's sure useful to test with.)
sudo apt update 
sudo apt install gnu-efi mtools qemu-system

# Install hot-reload dependencies
cd build/hotreload
yarn                  # (to install dependencies)
cd ../..
```

**Step 3: Build the system**

```bash
# Build the system
make clean
make thornhill
make emulator
```

**Step 4: Start the hot-reload toolchain**

```bash
# Start the hot-reload toolchain
make
```



## Commands

- `make clean`: Cleans the build and creates the initial directory structure, intended for preparation of an initial build.

- `make thornhill`: Builds the system.

- `make emulator`: Runs QEMU with the built-in QEMU EFI firmware and the system image.

- `make`: Starts the hot-reload toolchain.


## Troubleshooting

If, on WSL, QEMU returns “`Unable to init server: Could not connect: Connection refused`”, hover over the taskbar icon for VcXsrv to get the precise hostname and then change the value of `DISPLAY` in `~/.bashrc` to that. You can run `source ~/.bashrc` afterwards to apply this change.