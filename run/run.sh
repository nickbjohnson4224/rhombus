#!/bin/sh

export BUILDDIR=${PWD}

#bochs -qf ${BUILDDIR}/run/bochsrc.txt
qemu-system-i386 -cdrom ${BUILDDIR}/run/rhombus.iso -no-reboot -serial stdio -hda ${BUILDDIR}/run/boot.tar
