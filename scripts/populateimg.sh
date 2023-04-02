#!/usr/bin/env sh
printf "\tCopying kernel to system image...\n"
mcopy -D o -D O -i thornhill.img kernel ::/
