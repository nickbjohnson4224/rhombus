SOURCES := $(patsubst %.c,%.o,$(shell find . -name "*.c"))
SOURCES += $(BUILDDIR)/lib/c0.o
SUBDIR	:= apps/panel
TARGET	:= panel
CFLAGS  += -I$(BUILDDIR)/$(SUBDIR)/inc
LDFLAGS += -ltoolkit -lfreetype -llua -lc
ASFLAGS := -felf

all: panelui $(TARGET)

$(TARGET): $(SOURCES)
	@ echo " LD	" $(SUBDIR)/$(TARGET)
	@ $(LD) -o $(TARGET) $(SOURCES) $(LDFLAGS)
	@ echo " MV	" $(SUBDIR)/$(TARGET) "->" $(BUILDDIR)/bin
	@ mkdir -p $(BUILDDIR)/bin
	@ mv $(TARGET) $(BUILDDIR)/bin

%.o: %.c
	echo " CC	" $(SUBDIR)/$<
	@ $(CC) $(CFLAGS) -c $< -o $@

panelui:
	@ echo " CP	" $(SUBDIR)/panelui.txt "->" $(BUILDDIR)/etc
	@ mkdir -p $(BUILDDIR)/etc
	@ cp panelui.txt $(BUILDDIR)/etc

clean:
	@ rm $(TARGET) $(SOURCES)
