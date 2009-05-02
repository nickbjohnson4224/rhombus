SOURCES=table.o signal.o sysmap.o
CFLAGS=-nostdlib -nostdinc -fno-builtin -I../inc -fomit-frame-pointer
LDFLAGS=-r
ASFLAGS=-felf

all: $(SOURCES) link

link:
	ld $(LDFLAGS) -o task.o $(SOURCES)

clean:
	rm $(SOURCES) task.o

.s.o:
	nasm $(ASFLAGS) $<
