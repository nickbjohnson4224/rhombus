BUILDDIR=$(PWD)

LIB_DIRS=libc driver # libc must be first 
BIN_DIRS=kernel init

CC := /usr/flux-cross/bin/i586-elf-gcc
LD := /usr/flux-cross/bin/i586-elf-ld
AR := /usr/flux-cross/bin/i586-elf-ar
AS := nasm
PP := /usr/flux-cross/bin/i586-elf-cpp

CFLAGS	:= -march=i586 -pipe -Wall -Werror -Wextra -pedantic
CFLAGS	+= -Wpointer-arith -Wcast-align -Wwrite-strings
CFLAGS	+= -Wno-array-bounds -Wno-unused-parameter
CFLAGS	+= -O3 -fomit-frame-pointer
CFLAGS	+= -ffreestanding -I$(BUILDDIR)/inc
LDFLAGS := -L$(BUILDDIR)/lib
ARFLAGS := rcs
PPFLAGS := -x assembler-with-cpp -I$(BUILDDIR)/inc

export BUILDDIR CC LD AR AS PP CFLAGS LDFLAGS ARFLAGS PPFLAGS

.PHONY: $(LIB_DIRS) $(BIN_DIRS) clean test cd distclean makedirs

all: makedirs $(LIB_DIRS) $(BIN_DIRS)

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
	@ echo " CLEAN	" run/floppy.img run/khaos.iso
	@ rm run/floppy.img run/flux.iso
	@ echo " CLEAN	" $(shell find . -name "*.swp")
	@ rm $(shell find . -name "*.swp")

clean:
	@ echo " CLEAN	" $(shell find . -name "*.o")
	@ rm $(shell find . -name "*.o")
	@ echo " CLEAN	" $(shell find . -name "*.a")
	@ rm $(shell find . -name "*.a")
	@ echo " CLEAN	" $(shell find . -name "*.pp")
	@ rm $(shell find . -name "*.pp")
	@ echo " CLEAN	" bin/*
	@ rm bin/*

image:	all
	export BUILDDIR
	sudo run/image.sh

test:	all cd
	export BUILDDIR
	run/run.sh

cd:	all image
	export BUILDDIR
	run/makecd.sh

makedirs:
	mkdir -p $(BUILDDIR)/lib
	mkdir -p $(BUILDDIR)/inc
