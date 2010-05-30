BUILDDIR=$(PWD)

LIB_DIRS  = driver/pci

DRIVERS  := driver/term driver/kbd driver/tarfs
DAEMONS  := daemon/vfsd daemon/pmdd daemon/devd
UTILS    := util/ls util/echo

BIN_DIRS  = kernel init fish
BIN_DIRS += $(DRIVERS) $(DAEMONS) $(UTILS)

CC := clang
LD := ld
AR := ar
AS := nasm
PP := cpp

CFLAGS	:= -pipe -Wall -Werror -Wextra -pedantic -std=c99
CFLAGS	+= -Wpointer-arith -Wcast-align -Wwrite-strings
CFLAGS	+= -Wno-unused-parameter -Wno-unused-function
CFLAGS  += -mno-mmx
CFLAGS	+= -O3 -fomit-frame-pointer
CFLAGS	+= -ffreestanding -I$(BUILDDIR)/inc
LDFLAGS := -L$(BUILDDIR)/lib
ARFLAGS := rcs
PPFLAGS := -x assembler-with-cpp -I$(BUILDDIR)/inc

export BUILDDIR CC LD AR AS PP CFLAGS LDFLAGS ARFLAGS PPFLAGS

.PHONY: $(LIB_DIRS) $(BIN_DIRS) clean test cd distclean makedirs libc libflux

all: makedirs $(LIB_DIRS) $(BIN_DIRS) libc libflux

$(BIN_DIRS): $(LIB_DIRS) libc libflux
	@ echo " MAKE	" $@
	@ make -s -C $@

$(LIB_DIRS): libc libflux
	@ echo " MAKE	" $@
	@ make -s -C $@

libflux:
	@ echo " MAKE	" $@
	@ make -s -C $@

libc: libflux
	@ echo " MAKE	" $@
	@ make -s -C $@

distclean: clean
	@ echo " CLEAN	" $(shell find inc/*)
	@ rm -r inc/*
	@ echo " CLEAN	" run/floppy.img run/flux.iso
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
#	@ echo " CLEAN	" boot/*
#	@ rm boot/*

image:	all

	@ - mkdir boot
	@ cp bin/* boot
	@ strip boot/*

	@ export BUILDDIR
	@ sudo run/image.sh

test:	all cd
	@ export BUILDDIR
	@ run/run.sh

cd:	all image
	@ export BUILDDIR
	@ run/makecd.sh

hd:
	@ export BUILDDIR
	@ run/makehd.sh

makedirs:
	@ mkdir -p $(BUILDDIR)/lib
	@ mkdir -p $(BUILDDIR)/inc
