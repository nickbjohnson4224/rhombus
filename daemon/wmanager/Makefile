SOURCES := $(patsubst %.c,%.o,$(shell find . -name "*.c"))
SOURCES += $(patsubst %.s,%.o,$(shell find . -name "*.s"))
SOURCES += $(BUILDDIR)/lib/c0.o
SUBDIR	:= wmanager
TARGET	:= wmanager
CFLAGS  += -I$(BUILDDIR)/$(SUBDIR)/inc
LDFLAGS += -ldriver -lc
ASFLAGS := -felf

all: $(TARGET)

$(TARGET): $(SOURCES)
	@ echo " LD	" $(SUBDIR)/$(TARGET)
	@ $(LD) -o $(TARGET) $(SOURCES) $(LDFLAGS)
	@ echo " MV	" $(SUBDIR)/$(TARGET) "->" $(BUILDDIR)/sbin
	@ mkdir -p $(BUILDDIR)/sbin
	@ mv $(TARGET) $(BUILDDIR)/sbin

%.o: %.s
	echo " AS	" $(SUBDIR)/$<
	@ $(AS) $(ASFLAGS) $<

%.o: %.c
	echo " CC	" $(SUBDIR)/$<
	@ $(CC) $(CFLAGS) -c $< -o $@

clean:
	@ rm $(TARGET) $(SOURCES)
