export CC=/usr/khaos/bin/i586-elf-gcc
#export CC=tcc
FLAGS=-march=i586 -pipe -Wall -Werror -Wextra
export OPTS=$(FLAGS) -fomit-frame-pointer -O3
#export OPTS=$(FLAGS) -fomit-frame-pointer -Os

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
