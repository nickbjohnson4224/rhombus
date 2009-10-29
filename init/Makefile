SOURCES := $(patsubst %.c,%.o,$(shell find . -name "*.c"))
SOURCES += $(patsubst %.s,%.o,$(shell find . -name "*.s"))
SUBDIR	:= init
TARGET	:= init
CFLAGS  += -ffreestanding -I$(BUILDDIR)/$(SUBDIR)/inc
LDFLAGS += -lata -lconsole -lkhaos -lc -lpci -T$(BUILDDIR)/inc/link.ld
ASFLAGS := -felf

all: $(TARGET)

$(TARGET): $(SOURCES)
	echo " LD	" $(SUBDIR)/$(TARGET)
	@ $(LD) -o $(TARGET) $(SOURCES) $(LDFLAGS)
	echo " CP	" $(SUBDIR)/$(TARGET) "->" $(BUILDDIR)/bin
	@ cp $(TARGET) $(BUILDDIR)/bin

.s.o:
	echo " AS	" $(SUBDIR)/$<
	@ $(AS) $(ASFLAGS) $<

.c.o:
	echo " CC	" $(SUBDIR)/$<
	@ $(CC) $(CFLAGS) -c $< -o $@

clean:
	@ rm $(TARGET) $(SOURCES)
