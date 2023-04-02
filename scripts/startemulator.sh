#!/usr/bin/env sh

# Whether debugging flags should be set for the VM.
# Set to 0 by default for now.
Debug=0

# The amount of memory (directly passed to QEMU).
# 512M.
# 1024M or 1G (for 1GiB).
# 32768 or 32G (for 32GiB).
Memory="1024M"

# Identify the disk image to load.
ISOLocation="$1"                       # Just set to first command-line argument.

# Locate OVMF.
echo "Locating OVMF (UEFI firmware for virtual machine)..."

# Attempt to locate OVMF at any of the known locations.
for file in [ \
  "/usr/share/qemu/OVMF.fd"           `# Default location (Ubuntu)`           \
  "/usr/local/share/qemu/OVMF.fd"     `# Alternative Location (Ubuntu/WSL)`   \
  "/usr/share/edk2/x64/OVMF.fd"       `# Arch Linux`                          \
  "/opt/local/share/edk2/x64/OVMF.fd" `# macOS`                               \
]; do

  if [ -f "$file" ]; then
    OVMFLocation="$file"
  fi

done

# Now we'll check if we could find OVMF, halting if we couldn't.
if [ ! -f "$OVMFLocation" ]; then
  printf "Failed to locate OVMF\!\n\nYou can install this on Arch Linux with:\nsudo pacman -S edk2-ovmf\n\nOr, on Ubuntu with:\nsudo apt install ovmf"
  exit 1
fi

# If debug mode is set, add the debug flags.
if [ "$Debug" -eq 1 ]; then
  debugFlags="-D ./qemu-log.txt -d int"
fi

echo "Starting system with image $ISOLocation..."

# shellcheck disable=SC2086
# (globbing/splitting is desired)
qemu-system-x86_64 \
  -m "$Memory"                              \
  -qmp tcp:localhost:4444,server,nowait     \
  -bios "$OVMFLocation"                     \
  -drive "format=raw,file=$ISOLocation"     \
  -serial stdio                             \
  -monitor tcp::6969,server,nowait          \
  -no-reboot -no-shutdown                   \
  $debugFlags
