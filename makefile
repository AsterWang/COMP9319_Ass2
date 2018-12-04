CC = gcc
CFLAGS = -lm -Wall -g

all: bwtencode bwtsearch

bwtencode: bwtencode.c
	$(CC) $(CFLAGS) bwtencode.c -o bwtencode

bwtsearch: bwtsearch.c
	$(CC) $(CFLAGS) bwtsearch.c -o bwtsearch

clean:
		rm bwtsearch bwtencode
