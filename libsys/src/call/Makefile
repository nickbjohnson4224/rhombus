SOURCES=stubs.o table.o
CFLAGS=-nostdlib -nostdinc -fno-builtin -I../inc -fomit-frame-pointer
LDFLAGS=-r
ASFLAGS=-felf

all: $(SOURCES) link

link:
	ld $(LDFLAGS) -o call.o $(SOURCES)

clean:
	rm $(SOURCES) call.o

.s.o:
	nasm $(ASFLAGS) $<
