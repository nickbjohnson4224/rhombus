SOURCES=test.o main.o
CFLAGS=-nostdlib -nostdinc -fno-builtin -Iinc -fomit-frame-pointer
LDFLAGS=-R ../../libsys/src/libsys
ASFLAGS=-felf

all: $(SOURCES) link

link:
	ld $(LDFLAGS) -o init $(SOURCES)

clean:
	rm $(SOURCES) init

.s.o:
	nasm $(ASFLAGS) $<
