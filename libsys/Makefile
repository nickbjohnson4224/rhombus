SOURCES=elf.o exec.o stack.o
CFLAGS=-nostdlib -nostdinc -fno-builtin -Iinc -I$(BUILDDIR)/inc $(OPTS)
LDFLAGS=-r
ASFLAGS=-felf

all: $(SOURCES) link
	install -t $(BUILDDIR)/lib libsys.a -m 644
	install -t $(BUILDDIR)/inc inc/exec.h -m 644

link:
	$(LD) -o libsys.o $(SOURCES) $(LDFLAGS)
	ar rcs libsys.a libsys.o

clean:
	rm $(SOURCES) libsys.o libsys.a
	rm $(BUILDDIR)/lib/libsys.a
	rm $(BUILDDIR)/inc/exec.h

.s.o:
	nasm $(ASFLAGS) $<
