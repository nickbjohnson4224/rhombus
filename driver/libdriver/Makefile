SOURCES=port.o rmem.o
LDFLAGS=-r
ASFLAGS=-felf

all: $(SOURCES) link
	install -t $(BUILDDIR)/lib libdriver.a -m 644
	install -t $(BUILDDIR)/inc inc/driver.h -m 644

link:
	$(LD) -o libdriver.o $(SOURCES) $(LDFLAGS)
	ar rcs libdriver.a libdriver.o

clean:
	rm $(SOURCES) libdriver.o libdriver.a
	rm $(BUILDDIR)/lib/libdriver.a
	rm $(BUILDDIR)/inc/driver.h

.s.o:
	nasm $(ASFLAGS) $<
