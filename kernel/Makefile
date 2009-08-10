SOURCES=init/init.o lib/lib.o mem/mem.o trap/trap.o task/task.o
export CFLAGS=-nostdlib -nostdinc -fno-builtin -I../inc $(OPTS)
LDFLAGS=-Tlink.ld
ASFLAGS=-felf

all: inc/config.h $(SOURCES) link

lib/lib.o: lib/*.c inc/lib.h
	make -C lib

init/init.o: init/*.c init/*.s
	make -C init

mem/mem.o: mem/*.c inc/mem.h
	make -C mem

trap/trap.o: trap/*.c trap/*.s
	make -C trap

task/task.o: task/*.c inc/task.h
	make -C task

link:
	ld $(LDFLAGS) -o kernel $(SOURCES)

clean:
	make -C lib clean
	make -C init clean
	make -C mem clean
	make -C trap clean
	make -C task clean
	rm kernel
