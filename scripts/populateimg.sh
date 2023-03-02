#!/usr/bin/env sh
printf "\tCopying kernel to system image...\n"
mcopy -i thornhill.img kernel ::/
