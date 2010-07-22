SOURCES := $(patsubst %.c,%.o,$(shell find . -name "*.c"))
SOURCES += $(patsubst %.s,%.o,$(shell find . -mindepth 2 -name "*.s"))
CNAUGHT := c0.o
HEADERS := $(shell find inc -name "*.h")
HEADERS += inc/link.ld
SUBDIR	:= libc
TARGET	:= libc.a
CFLAGS  += -I$(BUILDDIR)/$(SUBDIR)/inc
LDFLAGS += -r
ASFLAGS := -felf

.PHONY: $(HEADERS) clean

all: $(TARGET)

$(HEADERS):
	echo " CP	" $(SUBDIR)/$@ "->" $(BUILDDIR)/inc
	@ cp $@ $(BUILDDIR)/inc

$(TARGET): $(HEADERS) $(SOURCES) $(CNAUGHT)
	echo " AR	" $(SUBDIR)/$(TARGET)
	@ $(AR) $(ARFLAGS) $(TARGET) $(shell find . -name "*.o")
	echo " CP	" $(SUBDIR)/$(TARGET) "->" $(BUILDDIR)/lib
	@ mkdir -p $(BUILDDIR)/inc
	@ cp $(TARGET) $(BUILDDIR)/lib
	@ cp $(CNAUGHT) $(BUILDDIR)/lib

%.o: %.s
	echo " AS	" $(SUBDIR)/$<
	@ $(AS) $(ASFLAGS) $<

%.o: %.c
	@ echo " CC	" $(SUBDIR)/$<
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	@ rm $(TARGET) $(SOURCES)
