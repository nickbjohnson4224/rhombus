SOURCES=main.o stubs.o
LDFLAGS=-r
ASFLAGS=-felf

all: $(SOURCES) link

link:
	ld $(LDFLAGS) -o setup.o $(SOURCES)

clean:
	rm $(SOURCES) setup.o

.s.o:
	nasm $(ASFLAGS) $<
