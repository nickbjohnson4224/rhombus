SOURCES := src/base/ftsystem.o src/base/ftinit.o src/base/ftdebug.o src/base/ftbase.o
SOURCES += src/base/ftbbox.o src/base/ftglyph.o
SOURCES += src/base/ftbdf.o src/base/ftbitmap.o src/base/ftcid.o src/base/ftfstype.o src/base/ftgasp.o src/base/ftgxval.o src/base/ftlcdfil.o src/base/ftmm.o src/base/ftotval.o src/base/ftpatent.o src/base/ftpfr.o src/base/ftstroke.o src/base/ftsynth.o src/base/fttype1.o src/base/ftwinfnt.o
SOURCES += src/bdf/bdf.o src/cff/cff.o src/cid/type1cid.o src/pcf/pcf.o src/pfr/pfr.o src/sfnt/sfnt.o src/truetype/truetype.o src/type1/type1.o src/type42/type42.o src/winfonts/winfnt.o
SOURCES += src/raster/raster.o src/smooth/smooth.o
SOURCES += src/autofit/autofit.o src/cache/ftcache.o src/gzip/ftgzip.o src/lzw/ftlzw.o src/gxvalid/gxvalid.o src/otvalid/otvalid.o src/psaux/psaux.o src/pshinter/pshinter.o src/psnames/psnames.o
SOURCES += fb_write.o
HEADERS := include/freetype include/ft2build.h
SUBDIR	:= ports/freetype-2.4.4
STATIC	:= libfreetype.a
CFLAGS  += -I$(BUILDDIR)/$(SUBDIR)/include -DFT2_BUILD_LIBRARY -w

.PHONY: $(HEADERS) clean

all: $(STATIC)

$(HEADERS):
	@ echo " CP	" $(SUBDIR)/$@ "->" $(BUILDDIR)/inc
	@ cp -r $@ $(BUILDDIR)/inc

$(STATIC): $(HEADERS) $(SOURCES)
	@ echo " AR	" $(SUBDIR)/$(STATIC)
	@ $(AR) $(ARFLAGS) $(STATIC) $(SOURCES)
	@ echo " CP	" $(SUBDIR)/$(STATIC) "->" $(BUILDDIR)/lib
	@ cp $(STATIC) $(BUILDDIR)/lib

%.o: %.c
	@ echo " CC	" $(SUBDIR)/$<
	@ $(CC) $(CFLAGS) -c $< -o $@

clean:
	@ rm $(STATIC) $(SOURCES)
