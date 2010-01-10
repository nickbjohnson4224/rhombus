SOURCES := $(patsubst %.c,%.o,$(shell find . -name "*.c"))
DRIVERS := $(patsubst %.c,%.a,$(shell find . -name "*.c"))
HEADERS := $(shell find inc -name "*.h")
SUBDIR	:= driver
CFLAGS  += -ffreestanding -I$(BUILDDIR)/driver/$(SUBDIR)/inc
LDFLAGS += -r

.PHONY: $(HEADERS) clean install

all: $(HEADERS) $(SOURCES) $(DRIVERS) install

$(HEADERS):
	@ echo " CP	" $(SUBDIR)/$@ "->" $(BUILDDIR)/inc/driver
	@ mkdir -p $(BUILDDIR)/inc/driver
	@ cp $@ $(BUILDDIR)/inc/driver

.c.o: $(HEADERS)
	@ echo " CC	" $(SUBDIR)/$<
	@ $(CC) $(CFLAGS) -c $< -o $@

.o.a: $(SOURCES)
	@ echo " AR	" $(SUBDIR)/$<
	@ $(AR) $(ARFLAGS) lib$@ $<
	@ echo " CP	" $(SUBDIR)/lib$@ "->" $(BUILDDIR)/lib
	@ cp lib$@ $(BUILDDIR)/lib	

clean:
	@ rm $(SOURCES) $(DRIVERS)
