SOURCES := $(patsubst %.c,%.o,$(shell find . -name "*.c"))
SOURCES += $(patsubst %.s,%.o,$(shell find . -name "*.s"))
SUBDIR	:= kernel
TARGET	:= kernel
CFLAGS  := -I$(BUILDDIR)/$(SUBDIR)/inc $(CFLAGS)
LDFLAGS := -Tinc/link.ld -melf_i386
ASFLAGS := -felf

all: $(TARGET)

$(TARGET): $(SOURCES)
	echo " LD	" $(SUBDIR)/$(TARGET)
	@ $(LD) -o $(TARGET) $(SOURCES) $(LDFLAGS)
	echo " MV	" $(SUBDIR)/$(TARGET) "->" $(BUILDDIR)/sbin
	@ mv $(TARGET) $(BUILDDIR)/sbin

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
