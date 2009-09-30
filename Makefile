BUILDDIR=$(PWD)

LIB_DIRS=libkernel libdriver libexec
BIN_DIRS=kernel init

CC := /usr/khaos/bin/i586-elf-gcc
LD := /usr/khaos/bin/i586-elf-ld
AR := /usr/khaos/bin/i586-elf-ar
AS := nasm

CFLAGS  := -march=i586 -pipe -Wall -Werror -Wextra -pedantic
CFLAGS  += -Wpointer-arith -Wcast-align -Wwrite-strings -Wno-unused-parameter
CFLAGS  += -fomit-frame-pointer -O3
CFLAGS	+= -I$(BUILDDIR)/inc
LDFLAGS := -L$(BUILDDIR)/lib
ARFLAGS := rcs

export BUILDDIR CC LD AR AS CFLAGS LDFLAGS ARFLAGS

.PHONY: $(LIB_DIRS) $(BIN_DIRS)

all: $(LIB_DIRS) $(BIN_DIRS)

$(BIN_DIRS): $(LIB_DIRS)
	@ echo " MAKE	" $@
	@ make -s -C $@

$(LIB_DIRS):
	@ export CFLAGS="$(CFLAGS) -fPIC"
	@ echo " MAKE	" $@
	@ make -s -C $@

clean:
	@ echo " CLEAN	" $(shell find . -name "*.o")
	@ rm $(shell find . -name "*.o")
	@ echo " CLEAN	libc"
	@ make -C libc clean

test:	all
	sudo run/test.sh
