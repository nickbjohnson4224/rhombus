SOURCES=std.o string.o printk.o panic.o pool.o
CFLAGS=-nostdlib -nostdinc -fno-builtin -I../inc -fomit-frame-pointer
LDFLAGS=-r
ASFLAGS=-felf

all: $(SOURCES) link

link:
	ld $(LDFLAGS) -o lib.o $(SOURCES)

clean:
	rm $(SOURCES) lib.o
