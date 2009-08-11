SOURCES=write.o read.o stubs.o
LDFLAGS=-r
ASFLAGS=-felf

all: $(SOURCES) link

link:
	ld $(LDFLAGS) -o cdrv.o $(SOURCES)

clean:
	rm $(SOURCES) cdrv.o

.s.o:
	nasm $(ASFLAGS) $<
