SOURCES=main.o stubs.o
LDFLAGS=-r
ASFLAGS=-felf

all: $(SOURCES) link

link:
	$(LD) -o setup.o $(SOURCES) $(LDFLAGS)

clean:
	rm $(SOURCES) setup.o

.s.o:
	nasm $(ASFLAGS) $<
