SOURCES=
CFLAGS=-nostdlib -nostdinc -fno-builtin -Iinc -fomit-frame-pointer
LDFLAGS=-r
ASFLAGS=-felf

all: $(SOURCES) link

link:
	ld $(LDFLAGS) -o exec.o $(SOURCES)

clean:
	rm exec.o

.s.o:
	nasm $(ASFLAGS) $<
