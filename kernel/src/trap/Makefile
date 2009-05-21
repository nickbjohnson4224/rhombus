SOURCES=idt.o int.o pit.o tss.o fault.o
CFLAGS=-nostdlib -nostdinc -fno-builtin -I../inc -fomit-frame-pointer
LDFLAGS=-r
ASFLAGS=-felf

all: $(SOURCES) link

link:
	ld $(LDFLAGS) -o trap.o $(SOURCES)

clean:
	rm $(SOURCES) trap.o

.s.o:
	nasm $(ASFLAGS) $<
