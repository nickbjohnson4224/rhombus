BUILDDIR=$(PWD)

DRV_DIRS=driver/console driver/floppy driver/ata driver/pci
LIB_DIRS=libkhaos libc $(DRV_DIRS)
BIN_DIRS=kernel init

CC := /usr/khaos/bin/i586-elf-gcc
LD := /usr/khaos/bin/i586-elf-ld
AR := /usr/khaos/bin/i586-elf-ar
AS := nasm

CFLAGS  := -march=i586 -pipe -Wall -Werror -Wextra -pedantic
CFLAGS  += -Wpointer-arith -Wcast-align -Wwrite-strings -Wno-unused-parameter
CFLAGS  += -O3 -fomit-frame-pointer
CFLAGS	+= -I$(BUILDDIR)/inc
LDFLAGS := -L$(BUILDDIR)/lib
ARFLAGS := rcs

export BUILDDIR CC LD AR AS CFLAGS LDFLAGS ARFLAGS

.PHONY: $(LIB_DIRS) $(BIN_DIRS) libc_headers

all: libc_headers $(LIB_DIRS) $(BIN_DIRS)

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

test:	all
	sudo run/test.sh

libc_headers:
	@ echo " CP	" $(shell find libc/inc -name "*.h") 
	@ echo "	  -> " $(BUILDDIR)/inc
	@ mkdir -p $(BUILDDIR)/inc
	@ cp libc/inc/*.h $(BUILDDIR)/inc
