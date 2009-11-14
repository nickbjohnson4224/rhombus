#!/bin/sh

export BUILDDIR=${PWD}

echo "c" | bochs -f ${BUILDDIR}/run/bochsrc.txt -q
