all: kernel libsys drivers

kernel:
	make -C kernel

libsys:
	make -C libsys

drivers:

clean:
	make -C kernel clean
	make -C libsys clean

test:
	
