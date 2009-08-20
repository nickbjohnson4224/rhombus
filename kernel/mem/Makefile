SOURCES=page.o map.o
#CFLAGS=-nostdlib -nostdinc -fno-builtin -I../inc -Wall -march=i386
LDFLAGS=-r
ASFLAGS=-felf

all: $(SOURCES) link

link:
	$(LD) -o mem.o $(SOURCES) $(LDFLAGS)

clean:
	rm $(SOURCES) mem.o

.s.o:
	nasm $(ASFLAGS) $<
