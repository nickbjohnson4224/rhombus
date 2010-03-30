SOURCES := $(patsubst %.c,%.o,$(shell find . -name "*.c"))
SOURCES += $(patsubst %.s,%.o,$(shell find . -name "*.s"))
SUBDIR	:= kernel
TARGET	:= kernel
CFLAGS  += -I$(BUILDDIR)/$(SUBDIR)/inc
LDFLAGS += -Tinc/link.ld
ASFLAGS := -felf

all: $(TARGET)

$(TARGET): $(SOURCES)
	echo " LD	" $(SUBDIR)/$(TARGET)
	@ $(LD) -o $(TARGET) $(SOURCES) $(LDFLAGS)
	echo " MV	" $(SUBDIR)/$(TARGET) "->" $(BUILDDIR)/bin
	@ mkdir -p $(BUILDDIR)/bin
	@ mv $(TARGET) $(BUILDDIR)/bin

%.o: %.s
	@ echo " PP	" $(SUBDIR)/$<
	@ $(PP) $(PPFLAGS) $< $<.pp
	@ echo " AS	" $(SUBDIR)/$<
	@ $(AS) $(ASFLAGS) $<.pp -o $@

%.o: %.c
	echo " CC	" $(SUBDIR)/$<
	@ $(CC) $(CFLAGS) -c $< -o $@

clean:
	@ rm $(TARGET) $(SOURCES)
