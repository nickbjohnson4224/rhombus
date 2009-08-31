SOURCES=string/arch_x86.o
export CFLAGS=-nostdlib -nostdinc -fno-builtin -Iinc -I$(BUILDDIR)/inc $(OPTS)
LDFLAGS=-r
ASFLAGS=-felf

all: $(SOURCES) link
	install -t $(BUILDDIR)/lib libc.a
	install -t $(BUILDDIR)/inc inc/string.h
	install -t $(BUILDDIR)/inc inc/stdint.h

link:
	$(LD) -o libc.o $(SOURCES) $(LDFLAGS)
	ar rcs libc.a libc.o

clean:
	rm libc.a libc.o $(SOURCES)

.s.o:
	nasm $(ASFLAGS) $<
