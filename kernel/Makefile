# Optimization flags
export CC=gcc
export OPTS=-march=i586 -fomit-frame-pointer

all:
	export CFLAGS
	make -C src

install: all
	./install.sh

test: all
	./test.sh

clean:
	make -C src clean
