CC = gcc
CFLAGS = -Wall -g
DEBUGFLAG = -DDEBUG=1
EXECUTABLE_NOM = mysh
OBJECT = mytinyshell.o parseurDeCommande.o utilitiesString.o utilitiesRegex.o ouvrirRepertoire.o memoirePartager.o redirection.o status.o mesJobs.o

all: tinyshell myls myps

myps: 
	$(CC) $(CFLAGS) src/myps.c -o myps

myls: 
	$(CC) $(CFLAGS) src/myls.c -o myls

debug: $(OBJECT)
	$(CC) $(CFLAGS) $(DEBUGFLAG) $(OBJECT) -o $(EXECUTABLE_NOM)

tinyshell: $(OBJECT)
	$(CC) $(CFLAGS) $(OBJECT) -o $(EXECUTABLE_NOM)

parseurDeCommande.o : lib/parseurDeCommande.c lib/parseurDeCommande.h
	$(CC) $(CFLAGS) -c lib/parseurDeCommande.c

utilitiesString.o : lib/utilitiesString.c  lib/utilitiesString.h
	$(CC) $(CFLAGS) -c lib/utilitiesString.c

utilitiesRegex.o : lib/utilitiesRegex.c  lib/utilitiesRegex.h
	$(CC) $(CFLAGS) -c lib/utilitiesRegex.c

ouvrirRepertoire.o : lib/ouvrirRepertoire.c lib/ouvrirRepertoire.h
	$(CC) $(CFLAGS) -c lib/ouvrirRepertoire.c

memoirePartager.o : lib/memoirePartager.c lib/memoirePartager.h
	$(CC) $(CFLAGS) -c lib/memoirePartager.c

redirection.o : lib/redirection.c lib/redirection.h
	$(CC) $(CFLAGS) -c lib/redirection.c

status.o : lib/status.c lib/status.h
	$(CC) $(CFLAGS) -c lib/status.c

mesJobs.o : lib/mesJobs.c lib/mesJobs.h
	$(CC) $(CFLAGS) -c lib/mesJobs.c

zip: clean save
	zip project SAVE/* SAVE/*/*

save:
	mkdir SAVE 2> /dev/null || true
	mkdir SAVE/src SAVE/lib 2> /dev/null || true
	cp -f Makefile ./SAVE
	cp -f *.* ./SAVE
	cp -f lib/* SAVE/lib
	cp -f src/* SAVE/src

clean:
	rm -rf *.o 2> /dev/null || true
	rm -rf */*.o 2> /dev/null || true
	rm shellId.txt 2> /dev/null || true
	rm mysh myls myps 2> /dev/null || true

clear: clean
	rm -f project.zip
	rm -rf SAVE/
