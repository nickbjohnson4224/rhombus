SOURCES := $(patsubst %.c,%.o,$(shell find . -name "*.c"))
SOURCES += $(patsubst %.s,%.o,$(shell find . -name "*.s"))
HEADERS := $(shell find inc -name "*.h")
SUBDIR	:= libtest
STATIC	:= libtest.a
SHARED	:= libtest.so
CFLAGS  += -I$(BUILDDIR)/$(SUBDIR)/inc
ASFLAGS := -felf

.PHONY: headers_dir $(HEADERS) clean

all: $(STATIC) $(SHARED)

headers_dir:
	@ echo " MKDIR	" $(BUILDDIR)/inc/rdi
	@ mkdir -p $(BUILDDIR)/inc/rdi

$(HEADERS): headers_dir
	@ echo " CP	" $(SUBDIR)/$@ "->" $(BUILDDIR)/inc
	@ cp $@ $(BUILDDIR)/$@

$(STATIC): $(HEADERS) $(SOURCES)
	@ echo " AR	" $(SUBDIR)/$(STATIC)
	@ $(AR) $(ARFLAGS) $(STATIC) $(SOURCES)
	@ echo " CP	" $(SUBDIR)/$(STATIC) "->" $(BUILDDIR)/lib
	@ cp $(STATIC) $(BUILDDIR)/lib

$(SHARED): $(HEADERS) $(SOURCES)
	@ echo " LD	" $(SUBDIR)/$(SHARED)
	@ $(LD) $(LDFLAGS) -shared $(SOURCES) -o $(SHARED) -Bdynamic -lc -soname libtest.so.1
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
