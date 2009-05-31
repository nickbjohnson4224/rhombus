SOURCES=
CFLAGS=-nostdlib -nostdinc -fno-builtin -I../inc -fomit-frame-pointer
LDFLAGS=-r
ASFLAGS=-felf

all: $(SOURCES) link

link:
	ld $(LDFLAGS) -o info.o $(SOURCES)

clean:
	rm info.o

.s.o:
	nasm $(ASFLAGS) $<
