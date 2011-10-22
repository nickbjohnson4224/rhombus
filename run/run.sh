#!/bin/sh

export BUILDDIR=${PWD}

qemu -cpu qemu32 -cdrom ${BUILDDIR}/run/rhombus.iso -no-reboot -serial stdio -hda ${BUILDDIR}/run/hd.img
