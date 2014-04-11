CC=gcc
CFLAGS=-Wall -O2
LDLIBS=-llisant

default: iptimetable_builder dump_ipspace

clean:
	rm -f *.o iptimetable_builder dump_ipspace

iptimetable_builder: histogram.o ipspace.o

dump_ipspace: ipspace.o
