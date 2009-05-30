SOURCES=init/init.o #call/call.o event/event.o exec/exec.o info/info.o
export CFLAGS=-nostdlib -nostdinc -fno-builtin -I../inc $(OPTS)
LDFLAGS=-Tlink.ld
ASFLAGS=-felf

all: $(SOURCES) link

init/init.o: init/*.c init/*.s inc/init.h
	make -C init

call/call.o: call/*.c inc/call.h
	make -C call

event/event.o: event/*.c inc/event.h
	make -C event

exec/exec.o: exec/*.c inc/exec.h
	make -C exec

info/info.o: info/*.c inc/info.h
	make -C info

link:
	ld $(LDFLAGS) -o libsys $(SOURCES)

clean:
	make -C init clean
#	make -C call clean
#	make -C event clean
#	make -C exec clean
#	make -C info clean
	rm libsys
