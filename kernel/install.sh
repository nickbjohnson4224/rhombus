#!/bin/bash

mount /dev/sda1 /boot
cp src/khaos /boot/khaos-0.1
cp ../run/initrd.tar /boot/initrd.tar
chmod 644 /boot/khaos-0.1
umount /boot
