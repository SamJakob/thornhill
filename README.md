# Thornhill



## Prerequisites

- Node.js

**Microsoft Windows:** you will need to install [Windows Subsystem for Linux 2 (WSL 2)](https://docs.microsoft.com/en-us/windows/wsl/install-win10) **and** the [VcXsrv X server](https://sourceforge.net/projects/vcxsrv/files/latest/download) for Windows. After you’ve installed VcXsrv, double click the `wsl-vcxsrv-config.xlaunch` file in the root of the repository. All of these commands should be executed from the WSL environment (easily accessed by opening a Command Prompt / PowerShell / Windows Terminal window and typing `wsl` at the prompt).



## Setup

**Step 1: Install [@rm-hull](https://github.com/rm-hull)’s Barebones Toolchain**

```bash
# Install Barebones Toolchain
mkdir ~/sdk
git clone https://github.com/rm-hull/barebones-toolchain ~/sdk/barebones-toolchain
echo ". ~/sdk/barebones-toolchain/setenv.sh" >> ~/.bashrc
source ~/.bashrc
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
sudo apt update
sudo apt install gnu-efi mtools qemu-system

# Install hot-reload dependencies
cd build/hotreload
npm install
cd ..
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