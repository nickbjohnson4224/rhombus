SOURCES=main.o setup.o
CFLAGS=-nostdlib -nostdinc -fno-builtin -Iinc -fomit-frame-pointer
LDFLAGS=-Tlink.ld
ASFLAGS=-felf

all: $(SOURCES) link

link:
	ld $(LDFLAGS) -o libsys $(SOURCES)

clean:
	rm libsys

.s.o:
	nasm $(ASFLAGS) $<
