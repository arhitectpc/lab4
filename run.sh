#!/usr/bin/env bash
set -e

KDIR="${KDIR:-/home/anton/Documents/Unik/labs/ak/repos/linux-stable}"
ROOTFS="$HOME/Documents/Unik/labs/ak/repos/busybox/rootfs.cpio.gz"
OUT=rootfs-lab4.cpio.gz

export KDIR
: "${ARCH:?ARCH не задан}"
: "${CROSS_COMPILE:?CROSS_COMPILE не задан}"

[ -f "$KDIR/arch/arm/boot/zImage" ] || { echo "Нет zImage в $KDIR"; exit 1; }
[ -f "$ROOTFS" ] || { echo "Нет базового rootfs: $ROOTFS"; exit 1; }

make

printf '%s\n' *.ko | cpio -o -H newc 2>/dev/null | gzip > extra.cpio.gz
cat "$ROOTFS" extra.cpio.gz > "$OUT"

qemu-system-arm -kernel "$KDIR/arch/arm/boot/zImage" \
  -initrd "$OUT" \
  -machine virt -nographic -m 512 \
  -append "root=/dev/ram0 rw console=ttyAMA0,115200 mem=512M"