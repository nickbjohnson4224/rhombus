#!/bin/sh

export BUILDDIR=${PWD}

#echo "c" | bochs -qf ${BUILDDIR}/run/bochsrc.txt
qemu -cpu qemu32 -cdrom ${BUILDDIR}/run/flux.iso -hda ${BUILDDIR}/run/hd.img -no-reboot
