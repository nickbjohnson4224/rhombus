BUILDDIR=$(PWD)

DRV_DIRS=driver/console driver/pci driver/floppy driver/ata
LIB_DIRS=libkhaos libc $(DRV_DIRS)
BIN_DIRS=kernel init

CC := /usr/khaos/bin/i586-elf-gcc
LD := /usr/khaos/bin/i586-elf-ld
AR := /usr/khaos/bin/i586-elf-ar
AS := nasm

CFLAGS  := -march=i586 -pipe -Wall -Werror -Wextra -pedantic
CFLAGS  += -Wpointer-arith -Wcast-align -Wwrite-strings -Wno-unused-parameter
CFLAGS  += -O1 -fomit-frame-pointer
CFLAGS	+= -I$(BUILDDIR)/inc
LDFLAGS := -L$(BUILDDIR)/lib
ARFLAGS := rcs

export BUILDDIR CC LD AR AS CFLAGS LDFLAGS ARFLAGS

.PHONY: $(LIB_DIRS) $(BIN_DIRS) clean test cd libc_headers distclean

all: libc_headers $(LIB_DIRS) $(BIN_DIRS)

$(BIN_DIRS): $(LIB_DIRS)
	@ echo " MAKE	" $@
	@ make -s -C $@

$(LIB_DIRS):
	@ export CFLAGS="$(CFLAGS) -fPIC"
	@ echo " MAKE	" $@
	@ make -s -C $@

distclean: clean
	@ echo " CLEAN	" $(shell find inc/*)
	@ rm -r inc/*
	@ echo " CLEAN	" run/floppy.img
	@ rm run/floppy.img

clean:
	@ echo " CLEAN	" $(shell find . -name "*.o")
	@ rm $(shell find . -name "*.o")
	@ echo " CLEAN	" $(shell find . -name "*.a")
	@ rm $(shell find . -name "*.a")
	@ echo " CLEAN	" bin/*
	@ rm bin/*

image:	all
	export BUILDDIR
	sudo run/image.sh

test:	all image
	export BUILDDIR
	run/run.sh

cd:	all image
	export BUILDDIR
	run/makecd.sh

libc_headers:
	@ echo " CP	" $(shell find libc/inc -name "*.*") "->" $(BUILDDIR)/inc
	@ mkdir -p $(BUILDDIR)/inc
	@ cp $(shell find libc/inc -name "*.*") $(BUILDDIR)/inc

