SUBDIR	:= driver
DRIVERS := pci terminal

.PHONY: $(DRIVERS) clean

all: $(DRIVERS)

$(DRIVERS):
	echo " MAKE	" $@
	make -s -C $@
