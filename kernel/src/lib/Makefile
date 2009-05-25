SOURCES=std.o string.o printk.o panic.o pool.o
LDFLAGS=-r
ASFLAGS=-felf

all: $(SOURCES) link

printk.o: printk.c
	cc $(CFLAGS) -O0 -c printk.c -o printk.o

link:
	ld $(LDFLAGS) -o lib.o $(SOURCES)

clean:
	rm $(SOURCES) lib.o
