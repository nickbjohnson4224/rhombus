SOURCES=table.o signal.o sched.o idle.o
#CFLAGS=-nostdlib -nostdinc -fno-builtin -I../inc -Wall -march=i386
LDFLAGS=-r
ASFLAGS=-felf

all: $(SOURCES) link

link:
	$(LD) -o task.o $(SOURCES) $(LDFLAGS)

clean:
	rm $(SOURCES) task.o

.s.o:
	nasm $(ASFLAGS) $<
