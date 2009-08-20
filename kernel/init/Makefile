SOURCES=boot.o main.o mem.o detect.o trap.o task.o kload.o elf.o
#CFLAGS=-nostdlib -nostdinc -fno-builtin -I../inc -Wall -march=i386 -fomit-frame-pointer
LDFLAGS=-r
ASFLAGS=-felf

all: $(SOURCES) link

link:
	$(LD) -o init.o $(SOURCES) $(LDFLAGS)

clean:
	rm $(SOURCES) init.o

.s.o:
	nasm $(ASFLAGS) $<
