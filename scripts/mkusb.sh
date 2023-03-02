#!/usr/bin/env bash
echo ""
echo "Locating platform USB creator..."

DIR=$(dirname "$0")

if [[ $OSTYPE == "darwin"* ]]; then
  . "$DIR/mkusb-osx.sh"
else
  . "$DIR/mkusb-linux.sh"
fi
