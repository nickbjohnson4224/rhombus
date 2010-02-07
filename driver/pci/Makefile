SOURCES := $(patsubst ./%.c,%.o,$(shell find . -name "*.c"))
HEADERS := $(shell find . -name "*.h")
TARGET  := libpci.a
SUBDIR	:= driver/pci
CFLAGS  += -ffreestanding -I$(BUILDDIR)/driver/$(SUBDIR)/inc
LDFLAGS += -r

.PHONY: $(HEADERS) clean install

all: $(HEADERS) $(SOURCES) $(TARGET)

$(HEADERS):
	@ echo " CP	" $(SUBDIR)/$@ "->" $(BUILDDIR)/inc/driver
	@ mkdir -p $(BUILDDIR)/inc/driver
	@ cp $@ $(BUILDDIR)/inc/driver

%.o: %.c $(HEADERS)
	@ echo " CC	" $(SUBDIR)/$<
	@ $(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(SOURCES)
	@ echo " AR	" $(SUBDIR)/$<
	@ $(AR) $(ARFLAGS) $@ $<
	@ echo " CP	" $(SUBDIR)/$@ "->" $(BUILDDIR)/lib
	@ cp $@ $(BUILDDIR)/lib	

clean:
	@ rm $(SOURCES) $(DRIVERS)
