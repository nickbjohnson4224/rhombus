SOURCES := $(patsubst %.c,%.o,$(shell find . -name "*.c"))
HEADERS := $(shell find inc -name "*.h")
SUBDIR	:= libtoolkit
STATIC	:= libtoolkit.a
CFLAGS  += -I$(BUILDDIR)/$(SUBDIR)/inc

.PHONY: headers_dir $(HEADERS) clean

all: $(STATIC)

headers_dir:
	@ echo " MKDIR	" $(BUILDDIR)/inc/toolkit
	@ mkdir -p $(BUILDDIR)/inc/toolkit	

$(HEADERS): headers_dir
	@ echo " CP	" $(SUBDIR)/$@ "->" $(BUILDDIR)/inc
	@ cp $@ $(BUILDDIR)/inc/toolkit/

$(STATIC): $(HEADERS) $(SOURCES)
	@ echo " AR	" $(SUBDIR)/$(STATIC)
	@ $(AR) $(ARFLAGS) $(STATIC) $(SOURCES)
	@ echo " CP	" $(SUBDIR)/$(STATIC) "->" $(BUILDDIR)/lib
	@ cp $(STATIC) $(BUILDDIR)/lib

%.o: %.c
	@ echo " CC	" $(SUBDIR)/$<
	@ $(CC) $(CFLAGS) -c $< -o $@

clean:
	@ rm $(TARGET) $(SOURCES)
