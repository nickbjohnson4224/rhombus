#!/bin/sh

export BUILDDIR=${PWD}

qemu -cpu qemu32 -cdrom ${BUILDDIR}/run/flux.iso -no-reboot
