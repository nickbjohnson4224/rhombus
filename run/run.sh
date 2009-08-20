#!/bin/sh
# updates the image and runs bochs

cp ../kernel/kernel .
cp ../init/init .
cp source_floppy.img floppy.img
/sbin/losetup /dev/loop4 floppy.img
mount /dev/loop4 mnt
tar -cvf initrd_old.tar init
cat initrd_old.tar null.tar > initrd.tar
cp initrd.tar mnt/initrd.tar
cp kernel mnt/kernel
umount /dev/loop4
echo "c" | bochs -f bochsrc.txt -q
/sbin/losetup -d /dev/loop4
