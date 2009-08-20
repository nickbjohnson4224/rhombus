SOURCES=page.o map.o
#CFLAGS=-nostdlib -nostdinc -fno-builtin -I../inc -Wall -march=i386
LDFLAGS=-r
ASFLAGS=-felf

all: $(SOURCES) link

link:
	ld $(LDFLAGS) -o mem.o $(SOURCES)

clean:
	rm $(SOURCES) mem.o

.s.o:
	nasm $(ASFLAGS) $<
