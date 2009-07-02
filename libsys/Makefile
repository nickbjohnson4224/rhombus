# Optimization flags
export CC=/usr/khaos/bin/i586-elf-gcc
export OPTS=-march=i586 -fomit-frame-pointer

all:
	export CFLAGS
	make -C src

install: all
	./install.sh

test: all
	make -C ../libsys
	./test.sh

clean:
	make -C src clean
