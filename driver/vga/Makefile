SOURCES := $(patsubst ./%.c,%.o,$(shell find . -name "*.c"))
SOURCES += $(BUILDDIR)/lib/c0.o
TARGET  := vga
SUBDIR	:= driver/vga
CFLAGS  += -I$(BUILDDIR)/$(SUBDIR)/inc
LDFLAGS += -lc -T$(BUILDDIR)/inc/link.ld
ASFLAGS += -felf

.PHONY: clean install

all: $(SOURCES) $(TARGET)

%.o: %.c $(HEADERS)
	@ echo " CC	" $(SUBDIR)/$<
	@ $(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(SOURCES)
	@ echo " LD	" $(SUBDIR)/$@
	@ $(LD) -o $(TARGET) $(SOURCES) $(LDFLAGS)
	@ echo " MV	" $(SUBDIR)/$@ "->" $(BUILDDIR)/bin
	@ mv $@ $(BUILDDIR)/bin

clean:
	@ rm $(SOURCES) $(TARGET)
