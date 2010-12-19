SOURCES := $(patsubst %.c,%.o,$(shell find . -name "*.c"))
SOURCES += $(patsubst %.s,%.o,$(shell find . -name "*.s"))
SUBDIR	:= dl
HEADERS := $(shell find . -name "*.h")
TARGET	:= dl.so
CFLAGS  += -fPIC
LDFLAGS	:= -T$(BUILDDIR)/$(SUBDIR)/link.ld
ASFLAGS := -felf

all: $(TARGET)

$(TARGET): $(HEADERS) $(SOURCES)
	@ echo " LD	" $(SUBDIR)/$(TARGET)
	@ $(LD) $(LDFLAGS) -shared $(SOURCES) -o $(TARGET)
	@ echo " CP	" $(SUBDIR)/$(TARGET) "->" $(BUILDDIR)/lib
	@ cp $(TARGET) $(BUILDDIR)/lib

%.o: %.s
	@ echo " AS	" $(SUBDIR)/$<
	@ $(AS) $(ASFLAGS) $<

%.o: %.c
	@ echo " CC	" $(SUBDIR)/$<
	@ $(CC) $(CFLAGS) -c $< -o $@
