#include "../src/quivontbien.h"

#include "memoirePartager.h"
#include "mesJobs.h"

int NUMBRE_DE_JOB = 0;

void mettreEnPauseUnProcessus(){

    printf("[%s] devient job n°%d", getenv("_"), NUMBRE_DE_JOB);
    kill(getpid(), SIGTSTP);
}

void remprendreUnProcessus(pid_t pid){

    kill(pid, SIGCONT);
}

int executeMyJobCommande(){

    return IGNORE_COMMANDE;
}