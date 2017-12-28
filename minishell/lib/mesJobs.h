#include <sys/types.h>

#ifndef __MES_JOBS_H__
#define __MES_JOBS_H__ 1

#define MYFG_STR "myfg"
#define MYBG_STR "mybg"
#define MYJOB_STR "myjobs"

#define STOPPER "Stoppé"
#define STOPPER_STR 'T'
#define ENCOUR  "En cours d'exécution"
#define ENCOUR_STR 'S'
#define INCONNUE "0"

#define NOUVELLE_CMD_BACKGROUND "[%d] %d\n"
#define TERMINER_CMD_BACKGROUND "%s (jobs=[%d], pid=%d) terminé avec status %d\n"
#define MAUVAIS_SYNC_SHM_CMDLINE "erreur de synchronisation de la MpJob et des pid actifs (skip)\n"
#define JUN_OB_AFFICHAGE "[%d] %d %22s %s\n"
#define PAUSE_PROCESSUS "[%s] devient job n°%d\n"
#define AUCUN_JOB_A_ID "Aucun job ne porte l'id %s (abandons)\n"
#define PROCESSUS_DEJA_EN_BG "Le processus est déjà en background (abandon)\n"

#define JOBIDFICHIER "/bin/sh"
#define CHIFFRE_DE_JOB_MAX 7

void detruireJobs();
void initialiserJobs();
void nouveauJobEnBackground();
void commandeEnBackgroundTermine();

void mettreEnPauseUnProcessus(int pid);

int executeMyJobCommande();

#endif