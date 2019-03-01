CC = gcc 
CFLAGS = -g -W
PATH := $(PATH):/home/deepesh/buildroot/buildroot/output/host/usr/bin


CFILES = pthreads.c
OBJS = ${CFILES:.c=.o}

all: pthreads

pthreads: pthreads.o
	$(CC) $(CFLAGS) -o $@ $@.o -lpthread -lrt

.c.o:
	$(CC) $(CFLAGS) -c $<


clean:
	-rm -f *.o
	-rm -f pthreads
	-rm -f log*.txt
