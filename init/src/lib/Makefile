SOURCES=string.o printk.o
LDFLAGS=-r
ASFLAGS=-felf

all: $(SOURCES) link

printk.o: printk.c
	cc $(CFLAGS) -O0 -c printk.c -o printk.o

link:
	ld $(LDFLAGS) -o lib.o $(SOURCES)

clean:
	rm $(SOURCES) lib.o
