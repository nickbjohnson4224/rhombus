#!/bin/sh

export BUILDDIR=${PWD}

qemu -cdrom ${BUILDDIR}/run/flux.iso -hda ${BUILDDIR}/run/hd.img -no-reboot
