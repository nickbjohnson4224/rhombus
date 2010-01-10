#!/bin/sh

export BUILDDIR=${PWD}

mkdir -p ${BUILDDIR}/run/cd
cp ${BUILDDIR}/run/floppy.img ${BUILDDIR}/run/cd
mkisofs -o ${BUILDDIR}/run/flux.iso -b floppy.img ${BUILDDIR}/run/cd
