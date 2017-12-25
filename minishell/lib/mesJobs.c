#include "../src/quivontbien.h"
#include "CONST_mytinyshell.h"

#include "memoirePartager.h"
#include "ouvrirRepertoire.h"
#include "utilitiesString.h"
#include "mesJobs.h"


int NUMBRE_DE_JOB = 0;
MemoirePartagerId idZoneJob;

void nouveauJobEnBackground(){

    printf("[%d] %d\n", NUMBRE_DE_JOB, getpid());
    signal(SIGINT, SIG_IGN);
}

void commandeEnBackgroundTermine(){

    printf("%s (jobs=[%d], pid=%d) terminé avec status %d\n", 
    NOM_DERNIER_PROCESSUS, 0, 0, CODE_DERNIERE_PROCESSUS);
    kill(getpid(), SIGTERM);
}

void affichageDesJobs(char *c){

    if (DEBUG) printf("[CONSOLE LOG] lecture de [%s]\n", c);

    char *d;
    int jobId = atoi(c), pid;

    for (d=c; *d && *d != EGAL; ++d);
    pid = atoi(++d);

    if ( !retournerLaCommandeViaPid(pid, &d)){
        ERREUR("erreur de synchronisation de la MpJob et des pid actifs (skip)\n");
    } else {
        printf("[%d] %d %20s %s\n", jobId, pid, "etat", d);
        free(d);
    }
}

void mettreEnPauseUnProcessus(int pid){

    char *c;
    char jobAscii[CHIFFRE_DE_JOB_MAX], pidAscii[CHIFFRE_DE_JOB_MAX];
    memset(jobAscii, 0, CHIFFRE_DE_JOB_MAX);
    memset(pidAscii, 0, CHIFFRE_DE_JOB_MAX);
    
    sprintf(jobAscii, "%d", NUMBRE_DE_JOB);
    sprintf(pidAscii, "%d", pid);
    printf("[%s] devient job n°%d\n", NOM_DERNIER_PROCESSUS, NUMBRE_DE_JOB++);
    c = fusionner3(jobAscii, "=", pidAscii);
    preformatAjouterUneValeurMemoirePartager(idZoneJob, c);
    free(c);
    signal(SIGTSTP, SIG_DFL);
    kill(pid, SIGTSTP);
}

int remprendreUnProcessus(char* jobAscii){

    int pidJobId;
    char *pidAscii;

    if ( obtenirLaValeurDuneClef(idZoneJob, jobAscii, &pidAscii) ){

        preformatSupprimerUneValeurMemoirePartager(idZoneJob, jobAscii);

        pidJobId = atoi(pidAscii); free(pidAscii);
        signal(SIGCONT, SIG_DFL);
        kill(pidJobId, SIGCONT);
        waitpid(pidJobId, NULL, 0);
        return 0;

    } else {

        printf("Aucun job ne porte l'id %s (abandons)\n", jobAscii);
    }

    return 1;

}

int executeMyJobCommande(char **commande){

    if (!strcmp(*commande, MYJOB_STR)){

        preformatAfficherMemoirePartager(idZoneJob, affichageDesJobs);
        return 0;

    } else if (!strcmp(*commande, MYFG_STR)){

        if ( estNull(*(commande+1)) ){ 
            ERREUR("commande incompléte, utiliser myfg <jobId> (abandon)\n"); 
            return 1;
        }
        
        return remprendreUnProcessus( *(commande+1) );

    } else if (!strcmp(*commande, MYBG_STR)){

        ERREUR("Commande non implémenté\n");
        return 1;
    }

    else return IGNORE_COMMANDE;
}

void initialiserJobs(){

    idZoneJob = creeEspaceDeMemoirePartager( genererUneClef(JOBIDFICHIER, 0) ,1);
}

void detruireJobs(){

    detruireMemoirePartager(idZoneJob);
}