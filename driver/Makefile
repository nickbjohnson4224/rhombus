SUBDIR	:= driver
DRIVERS := pci terminal keyboard ata

.PHONY: $(DRIVERS) clean

all: $(DRIVERS)

$(DRIVERS):
	echo " MAKE	" $@
	make -s -C $@
	
