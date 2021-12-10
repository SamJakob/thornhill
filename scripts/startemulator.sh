#!/bin/sh
#export DISPLAY
#DISPLAY=$(< /etc/resolv.conf grep nameserver | awk '{print $2; exit;}'):0.0

# Identify the disk image to load.
ISOLocation="$1"

# Locate OVMF.
echo Locating OVMF virtual BIOS...
# Attempt to locate OVMF at the default file location.
OVMFLocation="/usr/share/qemu/OVMF.fd"
# If it exists in /usr/local, then overwrite the path to check with that location.
if [ -f "/usr/local/share/qemu/OVMF.fd" ]; then
  OVMFLocation="/usr/local/share/qemu/OVMF.fd"
fi
# Now we'll check if we could find OVMF, halting if we couldn't.
if [ ! -f "$OVMFLocation" ]; then
  echo "Failed to locate OVMF!"
  exit 1
fi

echo "Starting system with image $ISOLocation..."
# qemu-system-x86_64 -qmp tcp:localhost:4444,server,nowait -bios "$OVMFLocation" "$ISOLocation" -monitor stdio -no-reboot -no-shutdown -D ./qemu-log.txt -d int
qemu-system-x86_64 -m 512M -qmp tcp:localhost:4444,server,nowait -bios "$OVMFLocation" "$ISOLocation" -monitor stdio
