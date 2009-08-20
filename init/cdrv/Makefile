SOURCES=write.o read.o stubs.o
LDFLAGS=-r
ASFLAGS=-felf

all: $(SOURCES) link

link:
	$(LD) -o cdrv.o $(SOURCES) $(LDFLAGS)

clean:
	rm $(SOURCES) cdrv.o

.s.o:
	nasm $(ASFLAGS) $<
