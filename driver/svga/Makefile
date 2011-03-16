SOURCES := $(patsubst ./%.c,%.o,$(shell find . -name "*.c"))
SOURCES += $(BUILDDIR)/lib/c0.o
TARGET  := svga
SUBDIR	:= driver/svga
CFLAGS  += -I$(BUILDDIR)/$(SUBDIR)/inc
LDFLAGS += -ldriver -lc
ASFLAGS += -felf

.PHONY: clean install

all: $(SOURCES) $(TARGET)

%.o: %.c $(HEADERS)
	@ echo " CC	" $(SUBDIR)/$<
	@ $(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(SOURCES)
	@ echo " LD	" $(SUBDIR)/$@
	@ $(LD) -o $(TARGET) $(SOURCES) $(LDFLAGS)
	@ echo " MV	" $(SUBDIR)/$@ "->" $(BUILDDIR)/sbin
	@ mv $@ $(BUILDDIR)/sbin

clean:
	@ rm $(SOURCES) $(TARGET)
