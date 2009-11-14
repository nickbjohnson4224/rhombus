#!/bin/sh

export BUILDDIR=${PWD}

mkdir ${BUILDDIR}/run/cd
cp ${BUILDDIR}/run/floppy.img ${BUILDDIR}/run/cd
mkisofs -o ${BUILDDIR}/run/khaos.iso -b floppy.img ${BUILDDIR}/run/cd
