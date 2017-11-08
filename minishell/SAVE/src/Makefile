CC = gcc
CFLAGS = -Wall -g
BINDIR = ../bin/
OBJECT = *.c

all: myls.c myls.h
	$(CC) $(CFLAGS) myls.c -o $(BINDIR)$a $<

clean:
	rm -rf *.o 2> /dev/null || true
