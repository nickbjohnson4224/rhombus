#!/bin/sh

export BUILDDIR=${PWD}

qemu-img create ${BUILDDIR}/run/hd.img 10M
