#include "src/quivontbien.h"

#include "lib/CONST_mytinyshell.h"
#include "lib/ouvrirRepertoire.h"
#include "lib/utilitiesString.h"
#include "lib/utilitiesRegex.h"
#include "lib/memoirePartager.h"
#include "lib/redirection.h"
#include "lib/status.h"
#include "lib/mesJobs.h"
#include "mytinyshell.h"

#define shmPPE_pidPExecChanger(i) (shmPPEChanger(i,idPidProcessusExec,pidProcessusExec,int*))
#define shmPPE_pidPExecObtenir(var) (shmPPEobtenir(var,idPidProcessusExec,pidProcessusExec,int*))

int *pidProcessusExec = NULL;

int shellId=0;
int CODE_DERNIERE_ARRET_OK = 0;
int CODE_DERNIERE_PROCESSUS = 0;
char *NOM_DERNIER_PROCESSUS = NULL;
MemoirePartagerId idLocal, idGlobal, idPidProcessusExec;

void changerPidExec(int pid){

	int i;
	shmPPE_pidPExecObtenir(i);
	if (i == 0) shmPPE_pidPExecChanger(pid);
}

void quitterShellProprement(){

	detruireMemoirePartager(idLocal);
	detruireJobs();

	shmctl(idPidProcessusExec, IPC_RMID,0);

	if (DEBUG){printf("[CONSOLE LOG] close ====== %d ======\n", shellId);}
	if (!shellId) detruireMemoirePartager(idGlobal);
}

void monSigInt2(){

	char c;
	printf("\nVoulez vous sortir du tinyshell et tuer de nombreux fils innoncents? (y/n): ");
	for (; (c = getchar()) ; ){
		if (c == MON_SININT_NO ) break; 
		if (c == MON_SININT_YES) kill(0, SIGTERM),exit(SIGINT);
	}

	printf("Appuyez sur entrer\n");
	for (; (c = getchar()) != EOF && c != '\n'; );
	AFFICHER_PROMPT();
}

void monSigTstp(){

	int valeur;
	pid_t pid;
	shmPPE_pidPExecObtenir(valeur);
	if ( valeur ){
		shmPPE_pidPExecChanger(0);
		mettreEnPauseUnProcessus(valeur);
		
		pid = fork();
		TESTFORKOK(pid);
		if (!pid) _exit(0);
	}
}

void monSigInt(){

	int valeur;
	shmPPE_pidPExecObtenir(valeur);
	if ( valeur ){
		shmPPE_pidPExecChanger(0);
		kill(valeur, SIGINT);
	}
	else monSigInt2();
	signal(SIGINT, monSigInt);

}

int executerCommandeExit(char **uneCommande){

	if ( !strcmp(*uneCommande,EXIT_STR) && estNull(*(uneCommande+1)) ){
		quitterShellProprement(); kill(getpid(), SIGTERM);
		return 1;	
	} else  return IGNORE_COMMANDE;
}

int executerMyCd(char **uneCommande){

	if ( !strcmp(*uneCommande, CD_STR) ){
		if ( estNull(*(uneCommande+1)) ) chdir(HOME_STR);
		else chdir( *(uneCommande+1) );
		return 1;
	}

	return IGNORE_COMMANDE;
}

char *derniereSousCommande(char **uneCommande){

	for (; *(uneCommande+1); uneCommande++);
	return *uneCommande;
}

int ee(char **c){

	int i;
	shmPPE_pidPExecObtenir(i);
	if (!strcmp(*c,"shmpid")) {printf("pid shm [%d]\n", i); return 1;}
	return IGNORE_COMMANDE;
}

int executeProgramme(char **uneCommande, int *entrer){

	pid_t pid;
	int status;
	int sortie[2];	
	int commandeTube = 0;
	int commandeBackground = 0;
	char **prochaineCommande = NULL;

	if ( estNull(uneCommande) ||estNull(*uneCommande) || estNull(**uneCommande) ){
		if ( estNull(entrer) ) return 0;
		else {
			char c;
			for (; read(entrer[0], &c, 1); putchar(c));
			close(entrer[0]);
			return 0;
		}
	}

	prochaineCommande = prochaineCommandeApresSeparateurStrict(uneCommande, "|");
	if ( nonNull(prochaineCommande) ){
		commandeTube = 1;
		*(prochaineCommande-1) = NULL;
		if ( pipe(sortie) == ERR ) FATALE_ERREUR("pipe -1\n", 126);	
		
	}

	if ( (status = executerCommandOperationSurLesVariables(VAR_LOCAL, uneCommande)) != IGNORE_COMMANDE ||
	     (status = executerCommandOperationSurLesVariables(VAR_GLOBAL, uneCommande)) != IGNORE_COMMANDE ||
		 (status = executerCommandStatus(uneCommande)) != IGNORE_COMMANDE ||
		 (status = executerCommandeExit(uneCommande)) != IGNORE_COMMANDE ||
		 (status = executeMyJobCommande(uneCommande)) != IGNORE_COMMANDE ||
		 (status = executerMyCd(uneCommande)) != IGNORE_COMMANDE ||
		 (status = ee(uneCommande)) != IGNORE_COMMANDE
		) return (status == 1)? 0 : 1;

	if ( !strcmp(derniereSousCommande(uneCommande),"&") ){

		commandeBackground = 1;
		pid = fork();
		TESTFORKOK(pid);

		if (pid) return 0;
			
	}

	pid = fork();
	TESTFORKOK(pid);

	CODE_DERNIERE_ARRET_OK = 1;
	CODE_DERNIERE_PROCESSUS = 127;
	free(NOM_DERNIER_PROCESSUS);
	NOM_DERNIER_PROCESSUS = NULL;
	
	if (!pid){

		if (DEBUG) printf("[CONSOLE LOG] pid execvp %d\n", getpid());

		if (commandeBackground) nouveauJobEnBackground();
		else shmPPE_pidPExecChanger(getpid());

		executeRedirectionSiBesoin(uneCommande);

		if ( nonNull(entrer) ) close(0),dup(entrer[0]),close(entrer[0]);
		
		if (commandeTube) close(1),dup(sortie[1]),close(sortie[1]);

		execvp(*uneCommande, uneCommande);
		fprintf(stderr, "Le programme [%s] n'existe pas\n", *uneCommande);
		_exit(127);
	
	} else {

		if ( commandeBackground ) signal(SIGINT, SIG_IGN);

		wait(&status);

		if ( nonNull(entrer) ) close(entrer[0]);
		if (commandeTube) close(sortie[1]);
		
		CODE_DERNIERE_ARRET_OK = (WIFEXITED(status))? 1 : 0;
		CODE_DERNIERE_PROCESSUS = (CODE_DERNIERE_ARRET_OK)? WEXITSTATUS(status) : ERR;
		NOM_DERNIER_PROCESSUS = chaineCopie(*uneCommande);
	
		if (commandeTube && !CODE_DERNIERE_PROCESSUS) return executeProgramme(prochaineCommande, sortie);
		
		if (commandeBackground) commandeEnBackgroundTermine();
		else shmPPE_pidPExecChanger(0);

		signal(SIGINT, monSigInt);
		return CODE_DERNIERE_PROCESSUS;
	
	}

	return -1;

}

int executeProchaineCommande(char ***prochaineCommande, char *operateur){

	char **analyseCommande = NULL;
	char **uneCommande = NULL;
	char operateurLogique = 0;
	char operateurDeFin = 0;

	if ( estNull(prochaineCommande) || estNull(*prochaineCommande) ) { 
		*operateur = 0;
		return 0;
	}

	for (analyseCommande = (*prochaineCommande); *analyseCommande; ){

		if (CARACTERE_BOOLEAN(*analyseCommande)){
			operateurLogique = *(*analyseCommande);
			*operateur = operateurLogique;
			*analyseCommande = NULL;
		} else if (CARACTERE_FINCOMMANDE(*analyseCommande)){
			operateurDeFin = *(*analyseCommande);
			*operateur = operateurDeFin;
			*analyseCommande = NULL;
		}
		else analyseCommande++;
	}

	uneCommande = (*prochaineCommande);
	(*prochaineCommande) = analyseCommande+1;

	if (DEBUG){printf("[CONSOLE LOG] Lancement de:");afficherTableauDeString(uneCommande);}

	return executeProgramme(uneCommande, NULL);

} 

int executeLesCommandes(char **pointerProchaineCommande){
	
	char operateurCommandeCourrantSuivante = 0;
	int resultatDerniereCommande = 1;
	int resultatDesCommandes = 0;
	int fini = 0;

	if ( estNull(pointerProchaineCommande) || estNull(*pointerProchaineCommande) ) return 0;

	for (; !fini ;){

		if (pointerProchaineCommande == NULL || *pointerProchaineCommande == NULL) break;
		resultatDerniereCommande =  executeProchaineCommande(&pointerProchaineCommande, &operateurCommandeCourrantSuivante);
		resultatDerniereCommande = (resultatDerniereCommande)? 0 : 1;

		if (pointerProchaineCommande == NULL || *pointerProchaineCommande == NULL) fini = 1;
		if (operateurCommandeCourrantSuivante == CODE_ET_LOGIQUE){
			if ( !resultatDerniereCommande) 
				pointerProchaineCommande = ignoreToutLesSeparateur(pointerProchaineCommande, "&&");
		} 
		else if (operateurCommandeCourrantSuivante == CODE_OU_LOGIQUE){
			if (resultatDerniereCommande)
			pointerProchaineCommande = ignoreToutLesSeparateur(pointerProchaineCommande, "||");
		} 	

		resultatDesCommandes = resultatDerniereCommande;
	}

	return resultatDesCommandes;
}


int bufferDepuisLentrerStandard(char **bufferDeSortie){
	
	int nombreDeCaractereLue = 0,
		nombreTotalDeCaractereLue = 0,
		tailleDuBuffer = ALLOCATION_CHAINE_TAILLE_BUFFER;
	
	char *buffer, *p,
		 *bufferDynamique;
	
	buffer = (char*) malloc(sizeof(char)*ALLOCATION_CHAINE_TAILLE_BUFFER);
	
	bufferDynamique = buffer;
	
	do {
		fgets(bufferDynamique, ALLOCATION_CHAINE_EXTEND_BUFFER, stdin);
		nombreDeCaractereLue = strlen(bufferDynamique);
	
		nombreTotalDeCaractereLue += nombreDeCaractereLue;
	
		if ( (tailleDuBuffer - ALLOCATION_CHAINE_EXTEND_BUFFER) < nombreTotalDeCaractereLue){
			p = (char*) realloc(buffer, tailleDuBuffer += 2*ALLOCATION_CHAINE_EXTEND_BUFFER);
			if (p == NULL) REALLOC_ERREUR(126);
			buffer = p;
		}
	
		bufferDynamique = buffer + nombreTotalDeCaractereLue;
	
	} while (nombreDeCaractereLue == ALLOCATION_CHAINE_EXTEND_BUFFER);
	
	if (nombreTotalDeCaractereLue == tailleDuBuffer){
		p = (char*) realloc(buffer, tailleDuBuffer + 1);
		if (p == NULL) REALLOC_ERREUR(126);
		buffer = p;
	} 
	
	memset(buffer+nombreDeCaractereLue, 0, tailleDuBuffer - nombreDeCaractereLue-1);
	
	*bufferDeSortie = buffer;
	return tailleDuBuffer;
	
}

int executerMinishell(int idLocal, int idGlobal){

	char** CommandesParLignes = NULL;
	char** bufferDeVidage;
	char*  bufferStdin = NULL;
	int    espaceDuBuffer, status = 1, fini = 0;

	signal(SIGINT, monSigInt);
	signal(SIGTSTP, monSigTstp);

	AFFICHER_PROMPT();
	espaceDuBuffer = bufferDepuisLentrerStandard(&bufferStdin);
	if (DEBUG) printf("commande entrer :[%s]\n", bufferStdin);

	if (*bufferStdin == RETOURALALIGNE){ free(bufferStdin); return 1;} 

	CommandesParLignes = ChaineVersTabDeChaineParReference(idLocal, idGlobal, bufferStdin);

	if ( *CommandesParLignes == NULL || *bufferStdin == RETOURALALIGNE);
	else if ( !strcmp(EXIT_STR, *CommandesParLignes)) fini = 1;
	else {
		status = executeLesCommandes(CommandesParLignes);
		if (DEBUG) printf("[CONSOLE LOG] :%s:\n", (status)? "SUCCES" : "FAILED");
	}

	for ( bufferDeVidage = CommandesParLignes; *bufferDeVidage ; ++bufferDeVidage){
		if (bufferStdin <= *bufferDeVidage && *bufferDeVidage <= bufferStdin + espaceDuBuffer) continue;
		else free(*bufferDeVidage);
	}

	free(CommandesParLignes);
	free(bufferStdin);
	return !fini;
}

int main(int argc, char** argv, char **envp){
	
	signal(SIGINT, monSigInt);
	signal(SIGTSTP, monSigTstp);

	idPidProcessusExec = MP_CREE(genererUneClef(PID_PROCESS_EXCEV_CLEF, getpid()), sizeof(int));
	shmPPE_pidPExecChanger(0);

	initialiserJobs();
	shellId = processPereEstUnTinyShell();
	if (DEBUG){printf("[CONSOLE LOG] open  ====== %d ====== file%d pere%d grop%d\n", 
	                shellId, getpid(), getppid(), getpgrp());}
	idLocal = creeEspaceDeMemoirePartager(genererUneClef(SHELLIDFICHIER, getpid()), 1);

	if (!shellId){
		idGlobal = creeEspaceDeMemoirePartager(genererUneClef(SHELLIDFICHIER, 1), 1);
		preformatAjouterDesValeurMemoirePartager(idGlobal, envp);

	} else idGlobal = creeEspaceDeMemoirePartager(genererUneClef(SHELLIDFICHIER, 1), 0);
	
	for (; executerMinishell(idLocal, idGlobal) ;);

	quitterShellProprement();
	exit(0);
}
