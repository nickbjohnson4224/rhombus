SOURCES := $(patsubst %.c,%.o,$(shell find . -name "*.c"))
SOURCES += $(patsubst %.s,%.o,$(shell find . -mindepth 2 -name "*.s"))
CNAUGHT := c0.o
HEADERS := $(shell find . -name "*.h")
HEADERS += inc/link.ld
SUBDIR	:= libc
STATIC	:= libc.a
SHARED	:= libc.so
CFLAGS  += -I$(BUILDDIR)/$(SUBDIR)/inc -fPIC
ASFLAGS := -felf

.PHONY: $(HEADERS) clean

all: $(STATIC) $(CNAUGHT) $(SHARED)

$(HEADERS):
	@ echo " CP	" $(SUBDIR)/$@ "->" $(BUILDDIR)/inc
	@ cp $@ $(BUILDDIR)/inc

$(STATIC): $(HEADERS) $(SOURCES)
	@ echo " AR	" $(SUBDIR)/$(STATIC)
	@ $(AR) $(ARFLAGS) $(STATIC) $(SOURCES)
	@ echo " CP	" $(SUBDIR)/$(STATIC) "->" $(BUILDDIR)/lib
	@ cp $(STATIC) $(BUILDDIR)/lib

$(CNAUGHT): $(patsubst %.o,%.s,$(CNAUGHT))
	@ echo " AS	" $(SUBDIR)/$<
	@ $(AS) $(ASFLAGS) $<
	@ echo " CP	" $(SUBDIR)/$(CNAUGHT) "->" $(BUILDDIR)/lib
	@ cp $(CNAUGHT) $(BUILDDIR)/lib

$(SHARED): $(HEADERS) $(SOURCES) $(CNAUGHT)
	@ echo " LD	" $(SUBDIR)/$(SHARED)
	@ $(LD) $(LDFLAGS) -shared $(SOURCES) -o $(SHARED)
	@ echo " CP	" $(SUBDIR)/$(SHARED) "->" $(BUILDDIR)/lib
	@ cp $(SHARED) $(BUILDDIR)/lib

%.o: %.s
	@ echo " AS	" $(SUBDIR)/$<
	@ $(AS) $(ASFLAGS) $<

%.o: %.c
	@ echo " CC	" $(SUBDIR)/$<
	@ $(CC) $(CFLAGS) -c $< -o $@

clean:
	@ rm $(TARGET) $(SOURCES)
