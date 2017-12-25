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

#define shmPPEobtenir(var) {MP_MAT(pidProcessusExec,int*,idPidProcessusExec);\
var = *pidProcessusExec;\
shmdt((char*)pidProcessusExec); }
#define shmPPEChanger(var) {MP_MAT(pidProcessusExec,int*,idPidProcessusExec);\
*pidProcessusExec = var;\
shmdt((char*)pidProcessusExec); }

int *pidProcessusExec = NULL;

int shellId=0;
int CODE_DERNIERE_ARRET_OK = 0;
int CODE_DERNIERE_PROCESSUS = 0;
char *NOM_DERNIER_PROCESSUS = NULL;
MemoirePartagerId idLocal, idGlobal, idPidProcessusExec;

void quitterShellProprement(){

	nettoieUneZoneDeMemoirePartager(idLocal);
	detruireMemoirePartager(idLocal);
	detruireJobs();

	shmctl(idPidProcessusExec, IPC_RMID,0);

	if (DEBUG){printf("[CONSOLE LOG] close ====== %d ======\n", shellId);}
	if (!shellId){
		nettoieUneZoneDeMemoirePartager(idGlobal);
		detruireMemoirePartager(idGlobal);
	}
}

void monSigInt2(){

	char c;
	printf("\nVoulez vous sortir du tinyshell et tuer de nombreux fils innoncents? (y/n): ");
	for (; (c = getchar()) ; ){
		if (c == MON_SININT_NO ) break; 
		if (c == MON_SININT_YES) kill(0, SIGTERM),exit(SIGINT);
	}

	for (; (c = getchar()) != '\n' && c != EOF; );
}

void monSigTstp(){

	int valeur;
	pid_t pid;
	shmPPEobtenir(valeur);
	if ( valeur ){
		shmPPEChanger(0);
		mettreEnPauseUnProcessus(valeur);
		
		pid = fork();
		TESTFORKOK(pid);
		if (!pid) _exit(0);
	}
}

void monSigInt(){

	int valeur;
	shmPPEobtenir(valeur);
	if ( valeur ){
		shmPPEChanger(0);
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

int executeProgramme(char **uneCommande){

	pid_t pid;
	int status;
	int commandeBackground = 0;

	if ( estNull(*uneCommande) || !(**uneCommande) ) return 0;

	if ( (status = executerCommandOperationSurLesVariables(VAR_LOCAL, uneCommande)) != IGNORE_COMMANDE ||
	     (status = executerCommandOperationSurLesVariables(VAR_GLOBAL, uneCommande)) != IGNORE_COMMANDE ||
		 (status = executerCommandStatus(uneCommande)) != IGNORE_COMMANDE ||
		 (status = executerCommandeExit(uneCommande)) != IGNORE_COMMANDE ||
		 (status = executeMyJobCommande(uneCommande)) != IGNORE_COMMANDE ||
		 (status = executerMyCd(uneCommande)) != IGNORE_COMMANDE 
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
		else shmPPEChanger(getpid());

		executeRedirectionSiBesoin(uneCommande);

		execvp(*uneCommande, uneCommande);
		fprintf(stderr, "Le programme %s n'existe pas\n", *uneCommande);
		_exit(127);
	
	} else {

		if ( commandeBackground ) signal(SIGINT, SIG_IGN);

		wait(&status);

		CODE_DERNIERE_ARRET_OK = (WIFEXITED(status))? 1 : 0;
		CODE_DERNIERE_PROCESSUS = (CODE_DERNIERE_ARRET_OK)? WEXITSTATUS(status) : ERR;
		NOM_DERNIER_PROCESSUS = chaineCopie(*uneCommande);
	
		if (commandeBackground) commandeEnBackgroundTermine();

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

	return executeProgramme(uneCommande);

}

int executeLesCommandes(char **pointerProchaineCommande){
	
	char operateurCommandeCourrantPrecedant = 0;
	char operateurCommandeCourrantSuivante = 0;
	int resultatDerniereCommande = 1;
	int resultatDesCommandes = 0;
	int fini = 0;

	if ( estNull(pointerProchaineCommande) || estNull(*pointerProchaineCommande) ) return 0;

	for (; !fini ;){

		resultatDerniereCommande =  executeProchaineCommande(&pointerProchaineCommande, &operateurCommandeCourrantSuivante);
		resultatDerniereCommande = (resultatDerniereCommande)? 0 : 1;

		if (pointerProchaineCommande == NULL || *pointerProchaineCommande == NULL) fini = 1;
		if (operateurCommandeCourrantPrecedant){
			if (operateurCommandeCourrantPrecedant == CODE_ET_LOGIQUE){
				if ( !(resultatDesCommandes  && resultatDerniereCommande) ) return 0;
			} else if (operateurCommandeCourrantSuivante == CODE_OU_LOGIQUE){
				if (resultatDerniereCommande) pointerProchaineCommande = ignoreToutLesSeparateur(pointerProchaineCommande, "||");
				resultatDesCommandes = resultatDesCommandes || resultatDerniereCommande;
				if (operateurCommandeCourrantSuivante == CODE_ET_LOGIQUE && !resultatDesCommandes) return 0;
			} else if (operateurCommandeCourrantPrecedant == CODE_FIN_LIGNE) continue;
		} else {
			if (operateurCommandeCourrantSuivante == CODE_ET_LOGIQUE && !resultatDerniereCommande) return 0;
			if (operateurCommandeCourrantSuivante == CODE_OU_LOGIQUE && resultatDerniereCommande) 
				pointerProchaineCommande = ignoreToutLesSeparateur(pointerProchaineCommande, "||");
			resultatDesCommandes = resultatDerniereCommande;
		}

		operateurCommandeCourrantPrecedant = operateurCommandeCourrantSuivante;
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

	idPidProcessusExec = MP_CREE(genererUneClef(PID_PROCESS_EXCEV_CLEF, 0), sizeof(int));
	shmPPEChanger(0);

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
