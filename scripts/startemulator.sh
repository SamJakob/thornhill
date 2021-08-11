#!/bin/bash
export DISPLAY=$(cat /etc/resolv.conf | grep nameserver | awk '{print $2; exit;}'):0.0
qemu-system-x86_64 -qmp tcp:localhost:4444,server,nowait -bios /usr/share/qemu/OVMF.fd "$1" -monitor stdio -no-reboot -no-shutdown -D ./qemu-log.txt -d int
# qemu-system-x86_64 -qmp tcp:localhost:4444,server,nowait -bios /usr/share/qemu/OVMF.fd "$1" -monitor stdio -no-reboot -no-shutdown