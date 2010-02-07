SUBDIR	:= driver
DRIVERS := pci ata terminal keyboard

.PHONY: $(DRIVERS) clean

all: $(DRIVERS)

$(DRIVERS):
	echo " MAKE	" $@
	make -s -C $@
	
