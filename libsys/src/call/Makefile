SOURCES=
CFLAGS=-nostdlib -nostdinc -fno-builtin -Iinc -fomit-frame-pointer
LDFLAGS=-r
ASFLAGS=-felf

all: $(SOURCES) link

link:
	ld $(LDFLAGS) -o call.o $(SOURCES)

clean:
	rm call.o

.s.o:
	nasm $(ASFLAGS) $<
