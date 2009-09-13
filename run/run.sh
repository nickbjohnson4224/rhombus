#!/bin/sh
# updates the image and runs bochs

cp ../kernel/kernel .
cp ../init/init .
cp source_floppy.img floppy.img
/sbin/losetup /dev/loop4 floppy.img
mount /dev/loop4 mnt
cp init mnt/init
cp kernel mnt/kernel
umount /dev/loop4
echo "c" | bochs -f bochsrc.txt -q
/sbin/losetup -d /dev/loop4
