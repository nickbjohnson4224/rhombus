SOURCES=std.o string.o printk.o panic.o pool.o memcpy.o
LDFLAGS=-r
ASFLAGS=-felf

all: $(SOURCES) link

link:
	$(LD) -o lib.o $(SOURCES) $(LDFLAGS)

clean:
	rm $(SOURCES) lib.o

.s.o:
	nasm $(ASFLAGS) $<
