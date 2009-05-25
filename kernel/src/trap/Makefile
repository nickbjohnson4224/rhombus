SOURCES=idt.o int.o pit.o tss.o syscall.o
#CFLAGS=-nostdlib -nostdinc -fno-builtin -I../inc -Wall -march=i386
LDFLAGS=-r
ASFLAGS=-felf

all: $(SOURCES) link

link:
	ld $(LDFLAGS) -o trap.o $(SOURCES)

clean:
	rm $(SOURCES) trap.o

.s.o:
	nasm $(ASFLAGS) $<
