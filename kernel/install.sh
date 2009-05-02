#!/bin/bash

sudo mount /dev/sda1 /boot
sudo cp src/khaos /boot/khaos-0.0
sudo chmod 644 /boot/khaos-0.0
sudo umount /boot
