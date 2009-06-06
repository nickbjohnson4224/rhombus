#!/bin/bash
# updates the image and runs bochs

cp ../kernel/src/khaos .
cp ../libsys/src/libsys .
cp ../init/src/init .
/sbin/losetup /dev/loop4 floppy.img
mount /dev/loop4 mnt
tar -cvf initrd.tar libsys init
cp initrd.tar mnt/initrd.tar
cp khaos mnt/kernel
umount /dev/loop4
bochs -f bochsrc.txt -q
/sbin/losetup -d /dev/loop4
