#include <sys/types.h>

#ifndef __MES_JOBS_H__
#define __MES_JOBS_H__ 1

#define MYFG_STR "myfg"
#define MYBG_STR "mybg"
#define MYJOB_STR "myjob"

#define JOBIDFICHIER "/blin/sh"
#define CHIFFRE_DE_JOB_MAX 7

void mettreEnPauseUnProcessus();
void remprendreUnProcessus(char* jobAscii);
int executeMyJobCommande();

#endif