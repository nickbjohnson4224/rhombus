SOURCES := $(patsubst %.c,%.o,$(shell find . -name "*.c"))
SOURCES += $(patsubst %.s,%.o,$(shell find . -name "*.s"))
HEADERS := $(shell find inc -name "*.h")
SUBDIR	:= libc
TARGET	:= libc.a
CFLAGS  += -ffreestanding -I$(BUILDDIR)/$(SUBDIR)/inc
LDFLAGS += -r -lkernel
ASFLAGS := -felf

.PHONY: $(HEADERS) clean

all: $(TARGET) $(HEADERS)

$(HEADERS):
	echo " CP	" $(SUBDIR)/$@ "->" $(BUILDDIR)/inc
	@ cp $@ $(BUILDDIR)/inc

$(TARGET): $(HEADERS) $(SOURCES) inc/*
	echo " AR	" $(SUBDIR)/$(TARGET)
	@ $(AR) $(ARFLAGS) $(TARGET) $(shell find . -name "*.o")
	echo " CP	" $(SUBDIR)/$(TARGET) "->" $(BUILDDIR)/lib
	@ cp $(TARGET) $(BUILDDIR)/lib

.s.o:
	echo " AS	" $(SUBDIR)/$<
	@ $(AS) $(ASFLAGS) $<

.c.o:
	echo " CC	" $(SUBDIR)/$<
	@ $(CC) $(CFLAGS) -c $< -o $@

clean:
	@ rm $(TARGET) $(SOURCES)
