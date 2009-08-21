export BUILDDIR=$(PWD)

export LD=/usr/khaos/bin/i586-elf-ld
export LIBRARY_PATH=$(PWD)/lib

export CC=/usr/khaos/bin/i586-elf-gcc
#export CC=tcc
FLAGS=-march=i586 -pipe -Wall -Werror -Wextra -Wshadow \
	-Wpointer-arith -Wcast-align -Wwrite-strings \
#	-Wmissing-declarations -Wredundant-decls \
#	-Winline -Wno-long-long -Wconversion -Wstrict-prototypes

# make sure GCC works with *all* of these flag sets
# also make sure TCC works with the last one
#export OPTS=$(FLAGS) -fomit-frame-pointer -O3
export OPTS=$(FLAGS) -fomit-frame-pointer -Os
#export OPTS=$(FLAGS) -fomit-frame-pointer -O0
#export OPTS=$(FLAGS) -O0

all:
	make -C kernel
	make -C driver
	make -C init

clean:
	make -C kernel clean
	make -C init clean
	make -C driver clean

test:	all
	sudo run/test.sh
