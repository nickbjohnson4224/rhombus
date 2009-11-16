#!/bin/sh

export BUILDDIR=${PWD}

cp ${BUILDDIR}/run/floppy.img ${BUILDDIR}/run/cd
mkisofs -o ${BUILDDIR}/run/khaos.iso -b floppy.img ${BUILDDIR}/run/cd
