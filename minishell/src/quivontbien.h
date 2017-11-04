#ifndef __LES_INCLUDES_QUI_VONT_BIEN_H__
#define __LES_INCLUDES_QUI_VONT_BIEN_H__ 1

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <dirent.h>
#include <assert.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

#define ERR -1
#define SHELLIDFICHIER "Makefile"
#define DROITS_DE_BASE 0600
#define ERREUR(s) perror(s)
#define FATALE_ERREUR(s,n) ERREUR(s),exit(n);
#define TESTFORKOK(pid) if(pid==ERR){fprintf(stderr,"fork-1 (%s).c,(%d)Li\n", __FILE__, __LINE__);exit(errno);}

#endif