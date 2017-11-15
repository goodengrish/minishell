#include "src/quivontbien.h"

#if !defined(DEBUG)
#define DEBUG 1
#endif

#include "lib/CONST_mytinyshell.h"
#include "lib/ouvrirRepertoire.h"
#include "lib/utilitiesString.h"
#include "lib/utilitiesRegex.h"
#include "lib/memoirePartager.h"
#include "lib/redirection.h"
#include "mytinyshell.h"

int executeProgramme(char **uneCommande){

	pid_t pid;
	int status;

	if (*uneCommande == NULL || !(**uneCommande) ) return 0;

	status = executerCommandOperationSurLesVariables(VAR_LOCAL, uneCommande);
	if (status != IGNORE_COMMANDE) return (status == 1)? 0 : 1;
	status = executerCommandOperationSurLesVariables(VAR_GLOBAL, uneCommande);
	if (status != IGNORE_COMMANDE) return (status == 1)? 0 : 1;

	pid = fork();
	TESTFORKOK(pid);
	
	if (!pid){

		executeRedirectionSiBesoin(uneCommande);

		execvp(*uneCommande, uneCommande);
		_exit(127);
	
	} else {
		wait(&status);
		return (WIFEXITED(status))? WEXITSTATUS(status) : ERR;
	
	}

	return -1;

}

int executeProchaineCommande(char ***prochaineCommande, char *operateur){

	char **analyseCommande = NULL;
	char **uneCommande = NULL;
	char operateurLogique = 0;
	char operateurDeFin = 0;

	if ( prochaineCommande == NULL || (*prochaineCommande) == NULL){ 
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

	if (pointerProchaineCommande == NULL || *pointerProchaineCommande == NULL) return 0;

	for (; !fini ;){

		resultatDerniereCommande =  executeProchaineCommande(&pointerProchaineCommande, &operateurCommandeCourrantSuivante);
		resultatDerniereCommande = (resultatDerniereCommande)? 0 : 1;

		if (pointerProchaineCommande == NULL || *pointerProchaineCommande == NULL) fini = 1;
		if (operateurCommandeCourrantPrecedant){
			if (operateurCommandeCourrantPrecedant == CODE_ET_LOGIQUE){
				if ( !(resultatDesCommandes  && resultatDerniereCommande) ) return 0;
			} else if (operateurCommandeCourrantPrecedant == CODE_OU_LOGIQUE){
				if (resultatDerniereCommande) pointerProchaineCommande = ignoreToutLesSeparateur(pointerProchaineCommande, "||");
				resultatDesCommandes = resultatDesCommandes || resultatDerniereCommande;
				if (operateurCommandeCourrantSuivante == CODE_ET_LOGIQUE && !resultatDesCommandes) return 0;
			} else if (operateurCommandeCourrantPrecedant == CODE_FIN_LIGNE) return resultatDerniereCommande;
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

	AFFICHER_PROMPT();
	espaceDuBuffer = bufferDepuisLentrerStandard(&bufferStdin);

	if (*bufferStdin == RETOURALALIGNE){ free(bufferStdin); return 1;} 

	CommandesParLignes = ChaineVersTabDeChaineParReference(idLocal, idGlobal, bufferStdin);

	if ( *CommandesParLignes == NULL || *bufferStdin == RETOURALALIGNE);
	else if ( !strcmp("exit", *CommandesParLignes)) fini = 1;
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

	int shellId=0;
    MemoirePartagerId idLocal, idGlobal;
	
	shellId = processPereEstUnTinyShell();
	if (DEBUG){printf("[CONSOLE LOG] open  ====== %d ====== file%d pere%d\n", shellId, getpid(), getppid());}
	idLocal = creeEspaceDeMemoirePartager(genererUneClef(SHELLIDFICHIER, getpid()), 1);

	if (!shellId){
		idGlobal = creeEspaceDeMemoirePartager(genererUneClef(SHELLIDFICHIER, 1), 1);
		preformatAjouterDesValeurMemoirePartager(idGlobal, envp);

	} else idGlobal = creeEspaceDeMemoirePartager(genererUneClef(SHELLIDFICHIER, 1), 0);
	
	for (; executerMinishell(idLocal, idGlobal) ;);

	nettoieUneZoneDeMemoirePartager(idLocal);
	detruireMemoirePartager(idLocal);

	if (DEBUG){printf("[CONSOLE LOG] close ====== %d ======\n", shellId);}
	if (!shellId){
		nettoieUneZoneDeMemoirePartager(idGlobal);
		detruireMemoirePartager(idGlobal);
	}

	exit(0);
}
