SOURCES := $(patsubst %.c,%.o,$(shell find . -name "*.c"))
SOURCES += $(patsubst %.s,%.o,$(shell find . -name "*.s"))
HEADERS := $(shell find inc -name "*.h")
SUBDIR	:= libmath
TARGET	:= libmath.a
CFLAGS  += -I$(BUILDDIR)/$(SUBDIR)/inc
LDFLAGS += -r -lkernel
ASFLAGS := -felf

.PHONY: $(HEADERS) clean

all: $(TARGET)

$(HEADERS):
	echo " CP	" $(SUBDIR)/$@ "->" $(BUILDDIR)/inc/math
	@ mkdir -p $(BUILDDIR)/inc/math
	@ cp $@ $(BUILDDIR)/inc/math

$(TARGET): $(HEADERS) $(SOURCES)
	echo " AR	" $(SUBDIR)/$(TARGET)
	@ $(AR) $(ARFLAGS) $(TARGET) $(shell find . -name "*.o")
	echo " CP	" $(SUBDIR)/$(TARGET) "->" $(BUILDDIR)/lib
	@ mkdir -p $(BUILDDIR)/inc
	@ cp $(TARGET) $(BUILDDIR)/lib

%.o: %.s
	echo " AS	" $(SUBDIR)/$<
	@ $(AS) $(ASFLAGS) $<

%.o: %.c
	@ echo " CC	" $(SUBDIR)/$<
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	@ rm $(TARGET) $(SOURCES)
