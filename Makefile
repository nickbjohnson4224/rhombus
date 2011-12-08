BUILDDIR=$(PWD)

LIB_DIRS  = libc dl librdi ports/lua ports/freetype libtoolkit

DRIVERS  := $(shell find driver -mindepth 1 -maxdepth 1)
DAEMONS  := $(shell find daemon -mindepth 1 -maxdepth 1)
UTILS    := $(shell find util   -mindepth 1 -maxdepth 1)
APPS     := $(shell find apps   -mindepth 1 -maxdepth 1)

BIN_DIRS  = kernel fish
BIN_DIRS += $(DRIVERS) $(DAEMONS) $(UTILS) $(APPS)

CC := clang -m32
LD := i586-pc-rhombus-ld
AR := i586-pc-rhombus-ar
AS := nasm
PP := cpp

CFLAGS	:= -pipe -Wall -Werror -Wextra -pedantic -std=c99
CFLAGS	+= -Wpointer-arith -Wwrite-strings
CFLAGS	+= -Wno-unused-parameter -Wno-unused-function
CFLAGS	+= -O3 -fomit-frame-pointer
CFLAGS	+= -I$(BUILDDIR)/inc -ffreestanding -fPIC
LDFLAGS := -L$(BUILDDIR)/lib -static
ARFLAGS := rcs
PPFLAGS := -x assembler-with-cpp -I$(BUILDDIR)/inc

PATH	:= $(BUILDDIR)/tools/bin:$(PATH)

export BUILDDIR CC LD AR AS PP CFLAGS LDFLAGS ARFLAGS PPFLAGS

.PHONY: $(LIB_DIRS) $(BIN_DIRS) clean test cd distclean makedirs ports

all: makedirs $(LIB_DIRS) $(BIN_DIRS) libc

$(LIB_DIRS):
	@ echo " MAKE	" $@
	@ make -s -C $@

$(BIN_DIRS): $(LIB_DIRS)
	@ echo " MAKE	" $@
	@ make -s -C $@

clean:
	@ echo " CLEAN	" $(shell find . -name "*.o")
	@ rm $(shell find . -name "*.o")
	@ echo " CLEAN	" $(shell find . -name "*.a")
	@ rm $(shell find . -name "*.a")
	@ echo " CLEAN	" $(shell find . -name "*.pp")
	@ rm $(shell find . -name "*.pp")
	@ echo " CLEAN	" bin/*
	@ rm bin/*
	@ echo " CLEAN	" $(shell find ./boot)
	@ rm -r boot/*
	@ echo " CLEAN	" $(shell find ./inc)
	@ rm -r inc/*

image:	all

	@ - mkdir boot
	@ - mkdir boot/bin
	@ - mkdir boot/lib
	@ - mkdir boot/sbin
	@ - mkdir boot/etc
	@ cp bin/* boot/bin
	@ cp lib/dl.so boot/lib
	@ cp lib/libc.so boot/lib
	@ cp sbin/* boot/sbin
#	@ cp -r etc/* boot/etc

	@ export BUILDDIR
	@ run/make_image.sh

test:	all image
	@ export BUILDDIR
	@ run/run.sh

makedirs:
	@ mkdir -p $(BUILDDIR)/lib
	@ mkdir -p $(BUILDDIR)/inc
	@ mkdir -p $(BUILDDIR)/driver
	@ mkdir -p $(BUILDDIR)/bin
	@ mkdir -p $(BUILDDIR)/sbin
