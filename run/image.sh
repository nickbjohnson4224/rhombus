#!/bin/bash

export BUILDDIR=${PWD}

cp ${BUILDDIR}/run/source_floppy.img ${BUILDDIR}/run/floppy.img
mount ${BUILDDIR}/run/floppy.img ${BUILDDIR}/run/mnt -o loop
cp ${BUILDDIR}/bin/init ${BUILDDIR}/run/mnt/init
cp ${BUILDDIR}/bin/kernel ${BUILDDIR}/run/mnt/kernel
umount ${BUILDDIR}/run/mnt
chmod 666 ${BUILDDIR}/run/floppy.img
