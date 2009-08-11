SOURCES=main.o stubs.o
LDFLAGS=-r
ASFLAGS=-felf

all: $(SOURCES) link

main.o: main.c
	$(CC) $(CFLAGS) -O0 -c -o main.o main.c

link:
	ld $(LDFLAGS) -o setup.o $(SOURCES)

clean:
	rm $(SOURCES) setup.o

.s.o:
	nasm $(ASFLAGS) $<
