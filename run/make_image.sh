#!/bin/bash

export BUILDDIR=${PWD}

mkdir -p ${BUILDDIR}/cd/boot/grub
cp ${BUILDDIR}/run/grub/* ${BUILDDIR}/run/cd/boot/grub

tar -C ${BUILDDIR}/boot -vvcf ${BUILDDIR}/run/boot.tar `ls ${BUILDDIR}/boot`
cp ${BUILDDIR}/run/boot.tar ${BUILDDIR}/run/cd/boot.tar
cp ${BUILDDIR}/bin/init ${BUILDDIR}/run/cd/init
cp ${BUILDDIR}/bin/kernel ${BUILDDIR}/run/cd/kernel

mkisofs -R -b boot/grub/stage2_eltorito -no-emul-boot -boot-load-size 4 -boot-info-table -o ${BUILDDIR}/run/flux.iso ${BUILDDIR}/run/cd
