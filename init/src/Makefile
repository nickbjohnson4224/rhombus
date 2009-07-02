SOURCES=lib/lib.o setup/setup.o
export CFLAGS=-nostdlib -nostdinc -fno-builtin -I../inc $(OPTS)
LDFLAGS=
ASFLAGS=-felf

all: $(SOURCES) link

link:
	ld $(LDFLAGS) -o init $(SOURCES)

lib/lib.o:
	make -C lib

setup/setup.o:
	make -C setup

clean:
	make -C lib clean
	make -C setup clean
	rm $(SOURCES) init
