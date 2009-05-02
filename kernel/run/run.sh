#!/bin/bash
# updates the image and runs bochs

cp ../src/khaos .
/sbin/losetup /dev/loop4 floppy.img
mount /dev/loop4 mnt
cp khaos mnt/kernel
umount /dev/loop4
bochs -f bochsrc.txt -q
/sbin/losetup -d /dev/loop4
