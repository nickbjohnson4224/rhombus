SUBDIR	:= driver
DRIVERS := pci terminal keyboard

.PHONY: $(DRIVERS) clean

all: $(DRIVERS)

$(DRIVERS):
	echo " MAKE	" $@
	make -s -C $@
	
