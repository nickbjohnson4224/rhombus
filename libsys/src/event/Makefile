SOURCES=
CFLAGS=-nostdlib -nostdinc -fno-builtin -I../inc -fomit-frame-pointer
LDFLAGS=-r
ASFLAGS=-felf

all: $(SOURCES) link

link:
	ld $(LDFLAGS) -o event.o $(SOURCES)

clean:
	rm event.o

.s.o:
	nasm $(ASFLAGS) $<
