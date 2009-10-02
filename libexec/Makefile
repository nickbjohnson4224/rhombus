SOURCES := $(patsubst %.c,%.o,$(shell find . -name "*.c"))
SOURCES += $(patsubst %.s,%.o,$(shell find . -name "*.s"))
HEADERS := inc/exec.h
SUBDIR	:= libexec
TARGET	:= libexec.a
CFLAGS  += -ffreestanding -I$(BUILDDIR)/$(SUBDIR)/inc -fPIC
LDFLAGS += -r
ASFLAGS := -felf

.PHONY: $(HEADERS) clean

all: $(TARGET) $(HEADERS)

$(HEADERS):
	echo " CP	" $(SUBDIR)/$@ "->" $(BUILDDIR)/inc
	@ cp $@ $(BUILDDIR)/inc

$(TARGET): $(HEADERS) $(SOURCES) inc/*
	echo " LD	" $(SUBDIR)/$*.o
	@ $(LD) -o $*.o $(SOURCES) $(LDFLAGS)
	echo " AR	" $(SUBDIR)/$(TARGET)
	@ $(AR) $(ARFLAGS) $(TARGET) $*.o
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
