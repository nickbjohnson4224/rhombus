# Optimization flags
#export CC=tcc
#export OPTS=-march=i586 -fomit-frame-pointer

all:
	export CFLAGS
	make -C src

install: all
	./install.sh

test: all
	make -C ../libsys
	make -C ../init
	./test.sh

clean:
	make -C src clean
