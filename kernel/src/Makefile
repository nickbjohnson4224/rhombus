SOURCES=init/init.o lib/lib.o mem/mem.o trap/trap.o task/task.o
CFLAGS=-nostdlib -nostdinc -fno-builtin -Iinc
LDFLAGS=-Tlink.ld
ASFLAGS=-felf

all: $(SOURCES) link

lib/lib.o: lib/*.c inc/lib.h
	make -C lib

init/init.o: init/*.c init/*.s inc/init.h
	make -C init

mem/mem.o: mem/*.c inc/mem.h
	make -C mem

trap/trap.o: trap/*.c trap/*.s inc/trap.h
	make -C trap

task/task.o: task/*.c inc/task.h
	make -C task

link:
	ld $(LDFLAGS) -o khaos $(SOURCES)

clean:
	make -C lib clean
	make -C init clean
	make -C mem clean
	make -C trap clean
	make -C task clean
	rm khaos
