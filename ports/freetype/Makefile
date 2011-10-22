DIR    := freetype-2.4.4
SOURCE := $(DIR).tar.bz2

all: $(DIR)
	@ make -s -C $(DIR)

$(SOURCE):
	@ echo " DL	" $(SOURCE)
	@ wget -c http://download.savannah.gnu.org/releases/freetype/$(SOURCE)

$(DIR): $(SOURCE)
	@ echo " UNPACK	" $(SOURCE)
	@ tar -xkf $(SOURCE)
	@ echo " CP	Makefile2 $(DIR)/Makefile"
	@ cp Makefile2 $(DIR)/Makefile

clean:
	@ rm -r $(DIR)
