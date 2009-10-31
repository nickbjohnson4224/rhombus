SOURCES := $(patsubst %.c,%.o,$(shell find . -name "*.c"))
SOURCES += $(patsubst %.s,%.o,$(shell find . -name "*.s"))
HEADERS := $(shell find inc -name "*.h")
SUBDIR	:= driver/console
TARGET	:= libconsole.a
CFLAGS  += -ffreestanding -I$(BUILDDIR)/driver/$(SUBDIR)/inc
LDFLAGS += -r
ASFLAGS := -felf

.PHONY: $(HEADERS) clean

all: $(TARGET) $(HEADERS)

$(HEADERS):
	echo " CP	" $(SUBDIR)/$@ "->" $(BUILDDIR)/inc/driver
	@ mkdir -p $(BUILDDIR)/inc/driver
	@ cp $@ $(BUILDDIR)/inc/driver

$(TARGET): $(HEADERS) $(SOURCES)
	echo " AR	" $(SUBDIR)/$(TARGET)
	@ $(AR) $(ARFLAGS) $(TARGET) $(shell find . -name "*.o")
	echo " CP	" $(SUBDIR)/$(TARGET) "->" $(BUILDDIR)/lib
	@ mkdir -p $(BUILDDIR)/lib
	@ cp $(TARGET) $(BUILDDIR)/lib

.s.o:
	echo " AS	" $(SUBDIR)/$<
	@ $(AS) $(ASFLAGS) $<

.c.o:
	echo " CC	" $(SUBDIR)/$<
	@ $(CC) $(CFLAGS) -c $< -o $@

clean:
	@ rm $(TARGET) $(SOURCES)
