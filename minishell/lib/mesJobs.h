#include <sys/types.h>

#ifndef __MES_JOBS_H__
#define __MES_JOBS_H__ 1

#define JOBIDFICHIER "mesJobs.h"
#define CHIFFRE_DE_JOB_MAX 7

void mettreEnPauseUnProcessus();
void remprendreUnProcessus(pid_t pid);
int executeMyJobCommande();

#endif