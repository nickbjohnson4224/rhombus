SOURCES=test.o
CFLAGS=-nostdlib -nostdinc -fno-builtin -Iinc -fomit-frame-pointer
LDFLAGS=
ASFLAGS=-felf

all: $(SOURCES) link

link:
	ld $(LDFLAGS) -o init $(SOURCES)

clean:
	rm $(SOURCES) init

.s.o:
	nasm $(ASFLAGS) $<
