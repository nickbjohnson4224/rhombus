SOURCES=init/init.o lib/lib.o call/call.o exec/exec.o event/event.o
export CFLAGS=-nostdlib -nostdinc -fno-builtin -I../inc $(OPTS)
LDFLAGS=-Tlink.ld
ASFLAGS=-felf

all: $(SOURCES) link

lib/lib.o: lib/*.c inc/lib.h
	make -C lib

init/init.o: init/*.c init/*.s inc/init.h
	make -C init

call/call.o: call/*.s inc/call.h
	make -C call

event/event.o: event/*.c inc/event.h
	make -C event

exec/exec.o: exec/*.c inc/exec.h
	make -C exec

event/event.o: event/*.c inc/event.h

link:
	ld $(LDFLAGS) -o libsys $(SOURCES)

clean:
	make -C init clean
	make -C lib clean
	make -C call clean
	make -C event clean
	make -C exec clean
#	make -C info clean
	rm libsys
