# Make a new empty FAT image with the required directory structure, if we don't already have one.
if [ ! -f "thornhill.img" ]; then
  dd if=/dev/zero of=thornhill.img bs=1048576 count=128
  mkfs.fat -v -F 32 -S 512 -s 1 thornhill.img -n THORNHILL
  mmd -i thornhill.img ::/EFI
  mmd -i thornhill.img ::/EFI/BOOT
fi

# Then regardless of the above, copy the newly generated bootloader in.
mcopy -n -o -i thornhill.img BOOTX64.EFI ::/EFI/BOOT