SOURCES=table.o signal.o sched.o
#CFLAGS=-nostdlib -nostdinc -fno-builtin -I../inc -Wall -march=i386
LDFLAGS=-r
ASFLAGS=-felf

all: $(SOURCES) link

link:
	ld $(LDFLAGS) -o task.o $(SOURCES)

clean:
	rm $(SOURCES) task.o

.s.o:
	nasm $(ASFLAGS) $<
