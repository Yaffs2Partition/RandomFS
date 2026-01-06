CC=gcc
CFLAGS=-Wall -O2
LIBS=`pkg-config fuse3 --cflags --libs`

randomfs:
	$(CC) $(CFLAGS) src/randomfs.c -o randomfs $(LIBS)

