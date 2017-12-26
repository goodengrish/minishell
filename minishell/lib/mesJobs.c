#include "../src/quivontbien.h"
#include "CONST_mytinyshell.h"

#include "memoirePartager.h"
#include "ouvrirRepertoire.h"
#include "utilitiesString.h"
#include "mesJobs.h"

#define shmPPE_ndjChanger(i) (shmPPEChanger(i,idNumbreDeJobs,NUMBRE_DE_JOBS,int*))
#define shmPPE_ndjObtenir(var) (shmPPEobtenir(var,idNumbreDeJobs,NUMBRE_DE_JOBS,int*))

int *NUMBRE_DE_JOBS;

MemoirePartagerId idZoneJob, idNumbreDeJobs;

int ajouterUnJob(int pid){

    int var;
    char *c;
    char jobAscii[CHIFFRE_DE_JOB_MAX], pidAscii[CHIFFRE_DE_JOB_MAX];

    memset(jobAscii, 0, CHIFFRE_DE_JOB_MAX);
    memset(pidAscii, 0, CHIFFRE_DE_JOB_MAX);
    
    shmPPE_ndjObtenir(var);

    sprintf(jobAscii, "%d", var++);
    sprintf(pidAscii, "%d", pid);
    c = fusionner3(jobAscii, "=", pidAscii);
    preformatAjouterUneValeurMemoirePartager(idZoneJob, c);

    shmPPE_ndjChanger(var);

    free(c);
    NOM_DERNIER_PROCESSUS = NULL;
    return var-1;
}

void nouveauJobEnBackground(){

    int var = ajouterUnJob(getpid());
    printf(NOUVELLE_CMD_BACKGROUND, var, getpid());
    signal(SIGINT, SIG_IGN);
}

void commandeEnBackgroundTermine(){

    printf(TERMINER_CMD_BACKGROUND, NOM_DERNIER_PROCESSUS, 0, getpid(), CODE_DERNIERE_PROCESSUS);
    kill(getpid(), SIGTERM);
}

void affichageDesJobs(char *jobIdstr){

    if (DEBUG) printf("[CONSOLE LOG] lecture de [%s]\n", jobIdstr);

    char *d;
    int jobId = atoi(jobIdstr), pid;

    for (d=jobIdstr; *d && *d != EGAL; ++d);
    pid = atoi(++d);

    if ( !retournerLaCommandeViaPid(pid, &d)){
        ERREUR(MAUVAIS_SYNC_SHM_CMDLINE);
    } else {
        if ( *d == '\0' ) preformatSupprimerUneValeurMemoirePartager(idZoneJob, jobIdstr);
        else printf(JUN_OB_AFFICHAGE, jobId, pid, STOPPER, d); 
        free(d);
    }
}

void mettreEnPauseUnProcessus(int pid){

    int var = ajouterUnJob(pid);
    char *nom;

    retournerLaCommandeViaPid(pid, &nom);
    printf(PAUSE_PROCESSUS, nom, var);
    free(nom);
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
        return 1;

    } else {

        printf(AUCUN_JOB_A_ID, jobAscii);
    }

    return 0;

}

int executeMyJobCommande(char **commande){

    if (!strcmp(*commande, MYJOB_STR)){

        preformatAfficherMemoirePartager(idZoneJob, affichageDesJobs);
        return 1;

    } else if (!strcmp(*commande, MYFG_STR)){

        if ( estNull(*(commande+1)) ){ 
            ERREUR("commande incompléte, utiliser myfg <jobId> (abandon)\n"); 
            return 0;
        }
        
        return remprendreUnProcessus( *(commande+1) );

    } else if (!strcmp(*commande, MYBG_STR)){

        ERREUR("Commande non implémenté\n");
        return 0;
    }

    else return IGNORE_COMMANDE;
}

void initialiserJobs(){

    idZoneJob = creeEspaceDeMemoirePartager( genererUneClef(JOBIDFICHIER, 0) ,1);
    idNumbreDeJobs = MP_CREE(genererUneClef(JOBIDFICHIER, 1), sizeof(int));
    shmPPE_ndjChanger(0);
}

void detruireJobs(){

    detruireMemoirePartager(idZoneJob);
    detruireMemoirePartager(idNumbreDeJobs);
}