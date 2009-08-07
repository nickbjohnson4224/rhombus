export CC=/usr/khaos/bin/i586-elf-gcc
#export CC=tcc
export OPTS=-march=i586 -fomit-frame-pointer -pipe -Wall -Werror -Wextra -Os

all:
	make -C kernel
	make -C libsys
	make -C init

kernel:
	make -C kernel

libsys:
	make -C libsys

init:
	make -C init

clean:
	make -C kernel clean
	make -C libsys clean
	make -C init clean

test:	all
	sudo run/test.sh
