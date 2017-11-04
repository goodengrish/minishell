CC = gcc
CFLAGS = -Wall -g
EXECUTABLE_NOM = tinyshell
OBJECT = mytinyshell.o allocationChaine.o utilitiesString.o ouvrirRepertoire.o memoirePartager.o

all: tinyshell

tinyshell: $(OBJECT)
	$(CC) $(CFLAGS) $(OBJECT) -o $(EXECUTABLE_NOM)

allocationChaine.o : lib/allocationChaine.c src/quivontbien.h lib/allocationChaine.h
	$(CC) $(CFAGS) -c lib/allocationChaine.c

utilitiesString.o : lib/utilitiesString.c  lib/utilitiesString.h
	$(CC) $(CFAGS) -c lib/utilitiesString.c

ouvrirRepertoire.o : lib/ouvrirRepertoire.c lib/ouvrirRepertoire.h
	$(CC) $(CFAGS) -c lib/ouvrirRepertoire.c

memoirePartager.o : lib/memoirePartager.c lib/memoirePartager.h
	$(CC) $(CFAGS) -c lib/memoirePartager.c

zip: clean save
	zip project SAVE/* SAVE/*/*

save:
	mkdir SAVE 2> /dev/null || true
	mkdir SAVE/src SAVE/lib 2> /dev/null || true
	mkdir SAVE/bin SAVE/bin 2> /dev/null || true
	cp -f Makefile ./SAVE
	cp -f *.* ./SAVE
	cp -f lib/* SAVE/lib
	cp -f src/* SAVE/src
	cp -f bin/* SAVE/bin

clean:
	rm -rf *.o 2> /dev/null || true
	rm -rf */*.o 2> /dev/null || true
	rm shellId.txt 2> /dev/null || true
	rm tinyshell 2> /dev/null || true

clear: clean
	rm -f project.zip
	rm -rf SAVE/
