#!/bin/sh

export BUILDDIR=${PWD}

qemu -cpu qemu32 -cdrom ${BUILDDIR}/run/rhombus.iso -no-reboot -serial stdio -hdd ${BUILDDIR}/run/hd.img
