SOURCES := $(patsubst %.c,%.o,$(shell find . -name "*.c"))
SOURCES += $(BUILDDIR)/lib/c0.o
SUBDIR	:= driver/tmpfs
TARGET	:= tmpfs
CFLAGS  += -I$(BUILDDIR)/$(SUBDIR)/inc
LDFLAGS += -lc -T$(BUILDDIR)/inc/link.ld
ASFLAGS := -felf

all: $(TARGET)

$(TARGET): $(SOURCES)
	@ echo " LD	" $(SUBDIR)/$(TARGET)
	@ $(LD) -o $(TARGET) $(SOURCES) $(LDFLAGS)
	@ echo " MV	" $(SUBDIR)/$(TARGET) "->" $(BUILDDIR)/sbin
	@ mv $(TARGET) $(BUILDDIR)/sbin

%.o: %.c
	echo " CC	" $(SUBDIR)/$<
	@ $(CC) $(CFLAGS) -c $< -o $@

clean:
	@ rm $(TARGET) $(SOURCES)
