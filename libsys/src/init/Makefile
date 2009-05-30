SOURCES=main.o setup.o
CFLAGS=-nostdlib -nostdinc -fno-builtin -Iinc -fomit-frame-pointer
LDFLAGS=-r
ASFLAGS=-felf

all: $(SOURCES) link

link:
	ld $(LDFLAGS) -o init.o $(SOURCES)

clean:
	rm init.o

.s.o:
	nasm $(ASFLAGS) $<
