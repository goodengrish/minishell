#include "../src/quivontbien.h"

#include "mesJobs.h"

int NUMBRE_DE_JOB = 0;

void mettreEnPauseUnProcessus(){

    kill(getpid(), SIGTSTP);
    printf("[%s] devient job n°%d", getenv("_"), NUMBRE_DE_JOB);
}

void remprendreUnProcessus(pid_t pid){

    kill(pid, SIGCONT);
}

int executeMyJobCommande(){

    return -2;
}