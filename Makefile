BUILDDIR=$(PWD)

LIB_DIRS  = libc libm

DRIVERS  := driver/term driver/kbd driver/tarfs driver/vga
DAEMONS  := daemon/init
UTILS    := util/ls util/echo util/halt util/cat
PORTS    := ports/lua

BIN_DIRS  = kernel fish
BIN_DIRS += $(DRIVERS) $(DAEMONS) $(UTILS) #$(PORTS)

CC := clang -arch=x86 -m32
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

PATH	:= $(BUILDDIR)/tools/bin:$(PATH)

export BUILDDIR CC LD AR AS PP CFLAGS LDFLAGS ARFLAGS PPFLAGS

.PHONY: $(LIB_DIRS) $(BIN_DIRS) clean test cd distclean makedirs ports

all: makedirs $(LIB_DIRS) $(BIN_DIRS) libc

$(BIN_DIRS): $(LIB_DIRS) libc libm
	@ echo " MAKE	" $@
	@ make -s -C $@

$(LIB_DIRS):
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

image:	all

	@ - mkdir boot
	@ cp bin/* boot

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
	@ mkdir -p $(BUILDDIR)/bin
