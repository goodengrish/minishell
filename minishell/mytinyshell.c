#include "src/quivontbien.h"

#include "lib/CONST_mytinyshell.h"
#include "lib/allocationChaine.h"
#include "lib/ouvrirRepertoire.h"
#include "lib/utilitiesString.h"
#include "lib/memoirePartager.h"
#include "lib/redirection.h"
#include "mytinyshell.h"

int executeProgramme(char **uneCommande){

	pid_t pid;
	int status;

	pid = fork();
	
	TESTFORKOK(pid);
	
	if (!pid){

		execvp(*uneCommande, uneCommande);
		_exit(127);
	
	} else {
		wait(&status);
		return (WIFEXITED(status))? WEXITSTATUS(status) : -1;
	
	}

	return -1;

}

int executeProchaineCommande(char ***prochaineCommande, char *operateur){

	char **analyseCommande = NULL;
	char **uneCommande = NULL;
	char operateurLogique = 0;
	char operateurDeFin = 0;
	int status = 1;
	int idLocal, idGlobal;

	if ( prochaineCommande == NULL || (*prochaineCommande) == NULL){ 
		*operateur = 0;
		return 0;
	}

	idLocal = creeEspaceDeMemoirePartager(genererUneClef(SHELLIDFICHIER, getpid()), 0);
	idGlobal =  creeEspaceDeMemoirePartager(genererUneClef(SHELLIDFICHIER, 1), 0);

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

	status = executerCommandOperationSurLesVariables(idLocal, VAR_LOCAL, uneCommande);
	if (status != -2) return (status == 1)? 0 : 1;
	status = executerCommandOperationSurLesVariables(idGlobal, VAR_GLOBAL, uneCommande);
	if (status != -2) return (status == 1)? 0 : 1;
	status = executeRedirection(uneCommande);
	if (status != -2) return (status == 1)? 0 : 1;
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
			
		assert(resultatDerniereCommande == 0 || resultatDerniereCommande == 1);

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

char **ChaineVersTabDeChaineParReference(MemoirePartagerId idLocal, MemoirePartagerId idGlobal, char *buffer){
    // ne pas free(buffer) sous paine d'incohérence des elements de sortie

    char **bufferCommandes = (char**) malloc(sizeof(char*)*TAILLE_SORTIE_DEFAUT);
	char *caractere;
	char *resultat;
	
	int arguments = 0;

	memset(bufferCommandes,0,TAILLE_SORTIE_DEFAUT);
	for (caractere = buffer ; *caractere && isspace(*caractere); ++caractere);
	for (arguments = 0 ; *caractere ; ++arguments){
		if ( CARACTERE_VARIABLE(caractere) ){
			ajouterLaTerminaisonALaPlaceDunEspace(caractere);
			if (obtenirLaValeurDuneClef(idLocal, caractere+1, &resultat)) bufferCommandes[arguments] = resultat;
			else if (obtenirLaValeurDuneClef(idGlobal, caractere+1, &resultat)) bufferCommandes[arguments] = resultat;
			else bufferCommandes[arguments] = NULL;
		}
		else bufferCommandes[arguments] = caractere;

		for (; *caractere && !isspace(*caractere); ++caractere){
			if (*caractere == CODE_FIN_LIGNE){
				bufferCommandes[++arguments] = ";"; 
				*caractere = '\0';  --caractere;
			}
		}
		*caractere++ = '\0';
		for (;*caractere && isspace(*caractere); ++caractere);
	}

	bufferCommandes[arguments] = NULL;
	
	afficherTableauDeString(bufferCommandes);

	return bufferCommandes;

}


int main(int argc, char** argv, char **envp){

	char** CommandesParLignes = NULL;
	char* bufferStdin = NULL;
	int fini = 0;
	int shellId=0;

    MemoirePartagerId idLocal, idGlobal;
	
	shellId = processPereEstUnTinyShell();
	printf("open  ====== %d ====== file%d pere%d\n", shellId, getpid(), getppid());
	idLocal = creeEspaceDeMemoirePartager(genererUneClef(SHELLIDFICHIER, getpid()), 1);

	if (!shellId){
		idGlobal = creeEspaceDeMemoirePartager(genererUneClef(SHELLIDFICHIER, 1), 1);
		preformatAjouterDesValeurMemoirePartager(idGlobal, envp);

	} else idGlobal = creeEspaceDeMemoirePartager(genererUneClef(SHELLIDFICHIER, 1), 0);
	
	for (; !fini;){

		putchar('~'); putchar('>'); putchar(' ');
		bufferStdin = bufferDepuisLentrerStandard();
		CommandesParLignes = ChaineVersTabDeChaineParReference(idLocal, idGlobal, bufferStdin);

		if (*bufferStdin == '\n' || *CommandesParLignes == NULL);
		else if ( !strcmp("exit", bufferStdin)) fini = 1;
		else if (*bufferStdin != '\n')printf(":%s:\n", executeLesCommandes(CommandesParLignes)? "SUCCES" : "FAILED");

		free(CommandesParLignes);
		free(bufferStdin);
	}

	nettoieUneZoneDeMemoirePartager(idLocal);
	detruireMemoirePartager(idLocal);

	printf("close ====== %d ======\n", shellId);
	if (!shellId){
		nettoieUneZoneDeMemoirePartager(idGlobal);
		detruireMemoirePartager(idGlobal);
	}

	exit(0);
}
