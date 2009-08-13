SOURCES=page.o map.o
CFLAGS=-nostdlib -nostdinc -fno-builtin -I../inc -Wall -march=i386
LDFLAGS=-r
ASFLAGS=-felf

all: $(SOURCES) link

map.o: map.c
	cc $(CFLAGS) -O0 -c -o map.o map.c

link:
	ld $(LDFLAGS) -o mem.o $(SOURCES)

clean:
	rm $(SOURCES) mem.o

.s.o:
	nasm $(ASFLAGS) $<
