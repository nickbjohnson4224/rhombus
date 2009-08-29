SOURCES=page.o map.o tlb.o
LDFLAGS=-r
ASFLAGS=-felf

all: $(SOURCES) link

link:
	$(LD) -o mem.o $(SOURCES) $(LDFLAGS)

clean:
	rm $(SOURCES) mem.o

.s.o:
	nasm $(ASFLAGS) $<
