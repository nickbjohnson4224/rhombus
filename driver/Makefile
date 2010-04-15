SUBDIR	:= driver
DRIVERS := pci

.PHONY: $(DRIVERS) clean

all: $(DRIVERS)

$(DRIVERS):
	echo " MAKE	" $@
	make -s -C $@
	
