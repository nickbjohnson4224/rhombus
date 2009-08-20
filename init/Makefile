SOURCES=lib/lib.o setup/setup.o
export CFLAGS=-nostdlib -nostdinc -fno-builtin -I../inc -I$(BUILDDIR)/inc $(OPTS)
LDFLAGS=-L$(BUILDDIR)/lib -ldriver
ASFLAGS=-felf

all: $(SOURCES) link

link:
	$(LD) -o init $(SOURCES) $(LDFLAGS)

lib/lib.o: lib/*.c
	make -C lib

setup/setup.o: setup/*.c setup/*.s
	make -C setup

cdrv/cdrv.o: cdrv/*.c
	make -C cdrv

clean:
	make -C lib clean
	make -C setup clean
	rm init
