#!/bin/sh
# runs bochs

/sbin/losetup /dev/loop4 temp.img
mount /dev/loop4 mnt
umount /dev/loop4
bochs -f bochsrc.txt -q
/sbin/losetup -d /dev/loop4
