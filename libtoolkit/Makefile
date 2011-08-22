SOURCES := $(patsubst %.c,%.o,$(shell find . -name "*.c"))
HEADERS := $(shell find inc -name "*.h" | grep -v private)
WIDGETS := $(shell find widgets -name "*.lua")
WINDOWS := $(shell find windows -name "*.txt")
SUBDIR	:= libtoolkit
STATIC	:= libtoolkit.a
CFLAGS  += -I$(BUILDDIR)/$(SUBDIR)/inc

.PHONY: headers_dir widgets_dir windows_dir themes_dir $(HEADERS) clean

all: $(STATIC)

headers_dir:
	@ echo " MKDIR	" $(BUILDDIR)/inc/toolkit
	@ mkdir -p $(BUILDDIR)/inc/toolkit

widgets_dir:
	@ echo " MKDIR	" $(BUILDDIR)/etc/widgets
	@ mkdir -p $(BUILDDIR)/etc/widgets

windows_dir:
	@ echo " MKDIR	" $(BUILDDIR)/etc/windows
	@ mkdir -p $(BUILDDIR)/etc/windows

themes_dir:
	@ echo " CP	" $(SUBDIR)/themes "->" $(BUILDDIR)/etc/themes
	@ rm -r $(BUILDDIR)/etc/themes
	@ cp -r $(BUILDDIR)/$(SUBDIR)/themes $(BUILDDIR)/etc/themes

$(HEADERS): headers_dir
	@ echo " CP	" $(SUBDIR)/$@ "->" $(BUILDDIR)/inc
	@ cp $@ $(BUILDDIR)/inc/toolkit/

$(WIDGETS): widgets_dir
	@ echo " CP	" $(SUBDIR)/$@ "->" $(BUILDDIR)/etc/widgets
	@ cp $@ $(BUILDDIR)/etc/widgets

$(WINDOWS): windows_dir
	@ echo " CP	" $(SUBDIR)/$@ "->" $(BUILDDIR)/etc/windows
	@ cp $@ $(BUILDDIR)/etc/windows

$(STATIC): themes_dir $(WIDGETS) $(WINDOWS) $(HEADERS) $(SOURCES)
	@ echo " AR	" $(SUBDIR)/$(STATIC)
	@ $(AR) $(ARFLAGS) $(STATIC) $(SOURCES)
	@ echo " CP	" $(SUBDIR)/$(STATIC) "->" $(BUILDDIR)/lib
	@ cp $(STATIC) $(BUILDDIR)/lib

%.o: %.c
	@ echo " CC	" $(SUBDIR)/$<
	@ $(CC) $(CFLAGS) -c $< -o $@

clean:
	@ rm $(TARGET) $(SOURCES)
