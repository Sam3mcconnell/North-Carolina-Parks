CC = gcc
CFLAGS = -D_GNU_SOURCE -Wall -std=c99 -g -lm

parks: parks.o catalog.o input.o
	$(CC) $(CFLAGS) -o parks parks.o catalog.o input.o
	
parks.o: parks.c catalog.h input.h
	$(CC) $(CFLAGS) -c parks.c

catalog.o: catalog.c catalog.h input.h
	$(CC) $(CFLAGS) -c catalog.c

input.o: input.c input.h
	$(CC) $(CFLAGS) -c input.c