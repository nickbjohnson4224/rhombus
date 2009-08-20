SOURCES=calls.o
LDFLAGS=-r
ASFLAGS=-felf

all: $(SOURCES) link
	install -t $(BUILDDIR)/lib libkernel.a -m 644
	install -t $(BUILDDIR)/inc inc/kernel.h -m 644

link:
	$(LD) -o libkernel.o $(SOURCES) $(LDFLAGS)
	ar rcs libkernel.a libkernel.o

clean:
	rm $(SOURCES) libkernel.o libkernel.a
	rm $(BUILDDIR)/lib/libkernel.a
	rm $(BUILDDIR)/inc/kernel.h

.s.o:
	nasm $(ASFLAGS) $<
