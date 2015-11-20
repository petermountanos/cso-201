CC = gcc
CFLAGS = -Wall

all: pagestats pagesim

pagestats: pagestats.c algorithms.c
	$(CC) $(CFLAGS)  pagestats.c algorithms.c -o pagestats

pagesim: pagesim.c algorithms.c
	$(CC) $(CFLAGS) pagesim.c algorithms.c -o pagesim

clean:
	rm -f pagestats pagesim
