SOURCES=main.o
CFLAGS=-nostdlib -nostdinc -fno-builtin -Iinc
LDFLAGS=-Tlink.ld
ASFLAGS=-felf

all: $(SOURCES) link

link:
	ld $(LDFLAGS) -o libsys $(SOURCES)

clean:
	rm libsys
