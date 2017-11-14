#include "src/quivontbien.h"

#if !defined(DEBUG)
#define DEBUG 0
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

char *extraieLaVariable(MemoirePartagerId idLocal, MemoirePartagerId idGlobal, char **car){

	char *resultat = NULL;
	char sauve;
	char *caractere = *car, *c = caractere+1;
	for (; *c && *c != ESPACE && !UN_CARACTERE_SEPARATEUR(*c) && *c != RETOURALALIGNE; ++c);
	sauve = *c; *c = ANTISLASHZERO;


	if (DEBUG){printf("[CONSOLE LOG] Var:[%s]", caractere);}

	if (obtenirLaValeurDuneClef(idLocal, caractere+1, &resultat));
	else if (obtenirLaValeurDuneClef(idGlobal, caractere+1, &resultat));
	else resultat = NULL;

	*c = sauve;
	*car = c;

	if (DEBUG){printf("=[%s]\n",resultat);}
	return resultat;
}

char *extraireLeSeparateur(char **caractere){

	char *separateur = (char*) malloc(sizeof(char)*4);
	char *tmp, *c;
	memset(separateur, 0, 4);

	c = *caractere;
	if (*c == '>' && *(c-1) == '2') --c;

	tmp = separateur;
	for (; *c && UN_CARACTERE_SEPARATEUR(*c) ; ++c){
		*tmp++ = *c; *c = ANTISLASHZERO;
	}

	*caractere = c;
	return separateur;
}

int compareDecalageAntislash(char caractere){

	return caractere == ANTISLASHZERO || caractere == ESPACE || caractere == RETOURALALIGNE || UN_CARACTERE_SEPARATEUR(caractere);
}

char **ChaineVersTabDeChaineParReference(MemoirePartagerId idLocal, MemoirePartagerId idGlobal, char *buffer){
    // ne pas free(buffer) sous paine d'incohérence des elements de sortie

    char **bufferCommandes = (char**) malloc(sizeof(char*)*TAILLE_SORTIE_DEFAUT);
	char *caractere;
	
	int arguments = 0, tailleMax = TAILLE_SORTIE_DEFAUT;

	if (UN_CARACTERE_SEPARATEUR(*buffer)){
		printf(MTSHELL_ERREUR_SYMBOLE_INNATENDU, *buffer);
		bufferCommandes[0] = NULL;
		*buffer = RETOURALALIGNE;
		return bufferCommandes;
	}

	memset(bufferCommandes,0,TAILLE_SORTIE_DEFAUT);
	for (caractere = buffer ; *caractere && isspace(*caractere); ++caractere);
	for (arguments = 0 ; *caractere ; ++arguments){

		if (*caractere == RETOURALALIGNE){*caractere = ANTISLASHZERO; continue;}

		if (*caractere == QUOTE){
			++caractere;
			bufferCommandes[arguments] = caractere;
			for (; *caractere && *caractere != QUOTE; ++caractere);
			if (*caractere == ANTISLASHZERO){
				printf(MTSHELL_ERREUR_ANTISLASH_VIDE); 
				*buffer=RETOURALALIGNE; return bufferCommandes;
			}
			*caractere = ANTISLASHZERO; ++caractere; continue;
		}

		if (arguments+2 == tailleMax){
			char **p = (char**) realloc(bufferCommandes, sizeof(char*)*(tailleMax+8));
			if (p == NULL) REALLOC_ERREUR(126);
			bufferCommandes = p; tailleMax += 8;
		}

		if (CARACTERE_VARIABLE(caractere)){
			bufferCommandes[arguments] = extraieLaVariable(idLocal, idGlobal, &caractere);
		}
		else if ( (buffer < caractere) && *(caractere-1) != ANTISLASH && UN_CARACTERE_SEPARATEUR(*caractere)){
			if (CARACTERE_SEPARATEUR_TOTAL(caractere)) 
				bufferCommandes[arguments] = extraireLeSeparateur(&caractere);
			else {
				printf(MTSHELL_ERREUR_SYMBOLE_INNATENDU, *caractere);
				*buffer = RETOURALALIGNE;
				return bufferCommandes;
			}
		}
		else {
			char *p;
			int contientdesAntiSlash = 0,
			    contientDesRegex = 0,
			    bloquerRegex = 0;

			bufferCommandes[arguments] = caractere;
			for (; !compareDecalageAntislash(*caractere) ; ){
				if (*caractere == ANTISLASH){
					++contientdesAntiSlash;
					if (*(caractere+1) == ANTISLASHZERO ||  *(caractere+1) == RETOURALALIGNE){
						printf(MTSHELL_ERREUR_SYMBOLE_INNATENDU, ANTISLASH);
						*buffer = RETOURALALIGNE;
						return bufferCommandes;
					} else if ( EST_UNE_CARACTERE_REGEX(*(caractere+1)) ) bloquerRegex = 1;
					++caractere;
				} 
				if ( EST_UNE_CARACTERE_REGEX(*(caractere+1)) ) ++contientDesRegex;
				++caractere;
			}
			if (*caractere == ESPACE || *caractere == RETOURALALIGNE) *caractere++ = ANTISLASHZERO;

			//supprimer les '\'
			if (contientdesAntiSlash){
				for (p = bufferCommandes[arguments]; *p; ++p){
					if (*p == ANTISLASH){ *(decalerDansLaMemeChaine(p+1,p)) = ANTISLASHZERO;}				
				}
			} else if(!bloquerRegex && contientDesRegex){
				
				int anciennePositionArgument = arguments;
				if (preformatExecuterRegex(&bufferCommandes, &arguments, &tailleMax, bufferCommandes[arguments]) == ERR ||
				      anciennePositionArgument == arguments){

					printf("Impossible d'acceder à '%s': Aucun fichier au dossier de ce genre\n", bufferCommandes[arguments]);
					*buffer = '\n';
					return bufferCommandes;
				}
			}
		}

		for (;*caractere && isspace(*caractere); ++caractere);
	}

	bufferCommandes[arguments] = NULL;

	if (DEBUG){printf("[CONSOLE LOG] Commande lue:");afficherTableauDeString(bufferCommandes);}

	return bufferCommandes;

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
		//nombreDeCaractereLue = read(fd, bufferDynamique, ALLOCATION_CHAINE_EXTEND_BUFFER);
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
	
	//printf("fd;%d & lu [%s] (%d caractéres), espace total de buffer %d\n", fd, buffer, nombreDeCaractereLue+1, tailleDuBuffer);
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
	if ( *CommandesParLignes == NULL);
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
