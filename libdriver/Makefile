SOURCES := $(patsubst %.c,%.o,$(shell find . -name "*.c"))
SOURCES += $(patsubst %.s,%.o,$(shell find . -name "*.s"))
HEADERS := $(shell find . -name "*.h")
SUBDIR	:= libdriver
TARGET	:= libdriver.a
CFLAGS  += -I$(BUILDDIR)/$(SUBDIR)/inc
ASFLAGS := -felf

.PHONY: $(HEADERS) clean

all: $(TARGET) $(CNAUGHT)

$(HEADERS):
	@ echo " CP	" $(SUBDIR)/$@ "->" $(BUILDDIR)/inc
	@ cp $@ $(BUILDDIR)/inc

$(TARGET): $(HEADERS) $(SOURCES)
	@ echo " AR	" $(SUBDIR)/$(TARGET)
	@ $(AR) $(ARFLAGS) $(TARGET) $(SOURCES)
	@ echo " CP	" $(SUBDIR)/$(TARGET) "->" $(BUILDDIR)/lib
	@ cp $(TARGET) $(BUILDDIR)/lib

%.o: %.s
	@ echo " AS	" $(SUBDIR)/$<
	@ $(AS) $(ASFLAGS) $<

%.o: %.c
	@ echo " CC	" $(SUBDIR)/$<
	@ $(CC) $(CFLAGS) -c $< -o $@

clean:
	@ rm $(TARGET) $(SOURCES)
