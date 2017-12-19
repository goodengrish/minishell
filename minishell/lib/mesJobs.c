#include "../src/quivontbien.h"

#include "memoirePartager.h"
#include "ouvrirRepertoire.h"
#include "utilitiesString.h"
#include "mesJobs.h"

int NUMBRE_DE_JOB = 0;
MemoirePartagerId idZoneJob;

void affichageDesJobs(char *c){

    char *d;
    int jobId = atoi(c), pid;

    for (d=c; *d && *d != '='; ++d);
    pid = atoi(d);

    if ( !retournerLaCommandeViaPid(pid, &d)){
        ERREUR("défaut de synchronisation de la MpJob et des pid actifs (skip)\n");
    } else {
        printf("[%d] %d %20s %s\n", jobId, pid, "etat", d);
        free(d);
    }
}

void mettreEnPauseUnProcessus(){

    char *c;
    char jobAscii[CHIFFRE_DE_JOB_MAX], pidAscii[CHIFFRE_DE_JOB_MAX];
    memset(jobAscii, 0, CHIFFRE_DE_JOB_MAX);
    memset(pidAscii, 0, CHIFFRE_DE_JOB_MAX);
    
    sprintf(jobAscii, "%d", NUMBRE_DE_JOB);
    sprintf(pidAscii, "%d", getpid());
    printf("[%s] devient job n°%d", getenv("_"), NUMBRE_DE_JOB++);
    c = fusionner3(jobAscii, "=", pidAscii);
    preformatAjouterUneValeurMemoirePartager(idZoneJob, c);
    free(c);
    signal(SIGTSTP, SIG_DFL);
    kill(getpid(), SIGTSTP);
}

void remprendreUnProcessus(int jobId){

    int pidJobId = -1;
    char *pidAscii = NULL;
    char jobAscii[CHIFFRE_DE_JOB_MAX];
    memset(jobAscii, 0, CHIFFRE_DE_JOB_MAX);

    sprintf(jobAscii, "%d", jobId);

    if ( obtenirLaValeurDuneClef(idZoneJob, jobAscii, &pidAscii) ){

        preformatSupprimerUneValeurMemoirePartager(idZoneJob, jobAscii);

        pidJobId = atoi(pidAscii); free(pidAscii);
        signal(SIGCONT, SIG_DFL);
        kill(pidJobId, SIGCONT);

    } else {

        printf("Aucun job ne porte l'id %d (abandons)\n", jobId);
        return;
    }

}


int executeMyJobCommande(char **commande){

    if (!strcmp(*commande, "myjob")){

        preformatAfficherMemoirePartager(idZoneJob, affichageDesJobs);
        return 0;
    }

    else return IGNORE_COMMANDE;
}

void initialiserJobs(){

    idZoneJob = creeEspaceDeMemoirePartager( genererUneClef(JOBIDFICHIER, 0) ,1);
}

void detruireJobs(){

    detruireMemoirePartager(idZoneJob);
}