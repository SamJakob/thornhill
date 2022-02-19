#!/bin/zsh
echo ""
echo "Thornhill | USB Creator for Linux Development Machines"
echo "Developed by NBTX"
echo ""

if [[ $OSTYPE == "darwin"* ]]; then
  echo "Invalid system. You should use mkusb-osx.sh on macOS machines."
  exit 255
fi

KERNEL_IMG=./out/thornhill.img

if [ ! -f "$KERNEL_IMG" ]; then
  echo "Failed to locate kernel image."
  exit 1
fi

echo ""
echo "-----------------"
echo "Disk Image ($KERNEL_IMG):"
ls -lh "$KERNEL_IMG"
echo "-----------------"
echo ""

# Attempt to get the default DRIVE and VOLUME using mount information.
DRIVE=`mount | grep THORNHILL | cut -d' ' -f1`
VOLUME=`mount | grep THORNHILL | cut -d' ' -f3`

# If the volume is not specified, require the user to select a disk.
if [ -z $VOLUME ]; then
  echo "Enter disk and partition:"
# Otherwise, allow them the option of leaving the input blank to select the default.
else
  echo "Enter disk and partition or press ENTER to select $DRIVE ($VOLUME):"
fi
read USER_DRIVE

# If USER_DRIVE is specified, override the drive.
if [ -n "$USER_DRIVE" ]; then
  DRIVE="$USER_DRIVE"
fi

if [ -z $DRIVE ]; then
  echo "Invalid disk."
  exit 1
fi

VOLUME=`mount | grep "$DRIVE" | cut -d' ' -f3`
if [ -z $VOLUME ]; then
  echo "The specified drive ($DRIVE) does not have a valid volume."
  exit 1
fi

if [[ $VOLUME == *$'\n'* ]]; then
  echo "You should select a disk and a partition, not an entire disk."
  echo "e.g., /dev/disk4s1, not /dev/disk4"
  echo "Aborting..."
  exit 1
fi

if [[ $VOLUME == '/' || $VOLUME == "/sys/"* ]]; then
  echo "This volume looks like a system disk!"
  echo "Aborting..."
  exit 1
fi

if [[ $VOLUME != "/run/media"* ]]; then
  echo "This does not look like removable media: $VOLUME"
  echo "Aborting..."
  exit 1
fi

echo -n "Are you sure you wish to proceed? This will erase the current contents of $DRIVE ($VOLUME)! [y/N] "
read response
case "$response" in
    [yY][eE][sS]|[yY])
        if [ `command -v gdd` ]; then
          echo "Requesting system privileges to unmount and perform gdd from $KERNEL_IMG to $DRIVE..."
          sudo diskutil unmount "$DRIVE"
          sudo gdd "if=$KERNEL_IMG" "of=$DRIVE" "bs=102400" "status=progress"
          echo "Done! The drive is now safe to remove."
        else
          echo "gdd command is unavailable. Falling back to dd. Progress will be unavailable."
          echo "Requesting system privileges to unmount and perform dd from $KERNEL_IMG to $DRIVE..."
          sudo diskutil unmount "$DRIVE"
          sudo dd "if=$KERNEL_IMG" "of=$DRIVE" "bs=102400"
          echo "Done! The drive is now safe to remove."
        fi
        ;;
    *)
        echo "Aborted by user."
        exit 2
        ;;
esac
