SOURCES=idt.o int.o call.o
#CFLAGS=-nostdlib -nostdinc -fno-builtin -I../inc -Wall -march=i386
LDFLAGS=-r
ASFLAGS=-felf

all: $(SOURCES) link

link:
	$(LD) -o trap.o $(SOURCES) $(LDFLAGS)

clean:
	rm $(SOURCES) trap.o

.s.o:
	nasm $(ASFLAGS) $<
