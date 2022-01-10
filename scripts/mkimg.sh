if [ ! -d `realpath $1` ] || [ ! -d `realpath $1`/root/ ]; then
  echo "You must specify an existing assets directory."
  echo "This should be done for you by the toolchain, so if you're seeing this something's wrong."
  exit 1
fi

ASSETS_DIR=`realpath $1`

# Make a new empty FAT image with the required directory structure, if we don't already have one.
if [ ! -f "thornhill.img" ]; then
  dd if=/dev/zero of=thornhill.img bs=1048576 count=128
  mkfs.fat -v -F 32 -S 512 -s 1 thornhill.img -n THORNHILL
  mmd -i thornhill.img ::/EFI
  mmd -i thornhill.img ::/EFI/BOOT
fi

# Then regardless of the above, copy the newly generated bootloader in.
echo "\tCopying bootloader application to system image..."
mcopy -n -o -i thornhill.img BOOTX64.EFI ::/EFI/BOOT

for file in $ASSETS_DIR/root/*; do
  if [ -e "$file" ]; then
    target=${file#$ASSETS_DIR/root/}
    echo "\tCopying asset to system image: $target"
    mcopy -n -o -i thornhill.img $file "::/$target"
  fi
done
