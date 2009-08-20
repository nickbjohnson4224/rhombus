SOURCES=string.o printk.o vga.o
LDFLAGS=-r
ASFLAGS=-felf

all: $(SOURCES) link

printk.o: printk.c
	cc $(CFLAGS) -O0 -c printk.c -o printk.o

link:
	$(LD) -o lib.o $(SOURCES) $(LDFLAGS)

clean:
	rm $(SOURCES) lib.o
