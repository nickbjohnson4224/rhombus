SOURCES=init/init.o lib/lib.o mem/mem.o trap/trap.o task/task.o
export CFLAGS=-nostdlib -nostdinc -fno-builtin -I$(BUILDDIR)/kernel/inc -I$(BUILDDIR)/inc $(OPTS)
LDFLAGS=-Tlink.ld
ASFLAGS=-felf

all: $(SOURCES) link

lib/lib.o: lib/*.c lib/*.s inc/lib.h
	make -C lib

init/init.o: init/*.c init/*.s
	make -C init

mem/mem.o: mem/*.c mem/*.s inc/mem.h
	make -C mem

trap/trap.o: trap/*.c trap/*.s
	make -C trap

task/task.o: task/*.c task/*.s inc/task.h
	make -C task

link:
	$(LD) -o kernel $(SOURCES) $(LDFLAGS)

clean:
	make -C lib clean
	make -C init clean
	make -C mem clean
	make -C trap clean
	make -C task clean
	rm kernel
	
