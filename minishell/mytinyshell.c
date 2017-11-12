#include "src/quivontbien.h"

#include "lib/CONST_mytinyshell.h"
#include "lib/ouvrirRepertoire.h"
#include "lib/utilitiesString.h"
#include "lib/memoirePartager.h"
#include "lib/redirection.h"
#include "mytinyshell.h"

int executeProgramme(char **uneCommande){

	pid_t pid;
	int status;

	if (*uneCommande == NULL || **uneCommande == '\0') return 0;

	status = executerCommandOperationSurLesVariables(VAR_LOCAL, uneCommande);
	if (status != -2) return (status == 1)? 0 : 1;
	status = executerCommandOperationSurLesVariables(VAR_GLOBAL, uneCommande);
	if (status != -2) return (status == 1)? 0 : 1;

	pid = fork();
	TESTFORKOK(pid);
	
	if (!pid){

		executeRedirectionSiBesoin(uneCommande);

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

	printf("[CONSOLE LOG] Lancement de:");afficherTableauDeString(uneCommande);

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

char *extraieLaVariable(MemoirePartagerId idLocal, MemoirePartagerId idGlobal, char **car){

	char *resultat = NULL;
	char sauve;
	char *caractere = *car, *c = caractere+1;
	for (; *c && *c != ' ' && !UN_CARACTERE_SEPARATEUR(*c) && *c != '\n'; ++c);
	sauve = *c; *c = '\0';


	printf("[CONSOLE LOG] Var:[%s]", caractere);

	if (obtenirLaValeurDuneClef(idLocal, caractere+1, &resultat));
	else if (obtenirLaValeurDuneClef(idGlobal, caractere+1, &resultat));
	else resultat = NULL;

	*c = sauve;
	*car = c;

	printf("=[%s]\n",resultat);
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
		*tmp++ = *c; *c = '\0';
	}

	*caractere = c;
	return separateur;
}

int compareDecalageAntislash(char caractere){

	return caractere == '\0' || caractere == ' ' || caractere == '\n' || UN_CARACTERE_SEPARATEUR(caractere);
}

char **ChaineVersTabDeChaineParReference(MemoirePartagerId idLocal, MemoirePartagerId idGlobal, char *buffer){
    // ne pas free(buffer) sous paine d'incohérence des elements de sortie

    char **bufferCommandes = (char**) malloc(sizeof(char*)*TAILLE_SORTIE_DEFAUT);
	char *caractere;
	
	int arguments = 0, tailleMax = TAILLE_SORTIE_DEFAUT;

	if (UN_CARACTERE_SEPARATEUR(*buffer)){
		printf("erreur de syntaxe prés du symbole inattendu %c\n", *buffer);
		bufferCommandes[0] = NULL;
		*buffer = '\n';
		return bufferCommandes;
	}

	memset(bufferCommandes,0,TAILLE_SORTIE_DEFAUT);
	for (caractere = buffer ; *caractere && isspace(*caractere); ++caractere);
	for (arguments = 0 ; *caractere ; ++arguments){

		if (*caractere == '\n'){*caractere = '\0'; continue;}

		if (*caractere == '"'){
			++caractere;
			bufferCommandes[arguments] = caractere;
			for (; *caractere && *caractere != '"'; ++caractere);
			if (*caractere == '\0'){
				printf("Erreur symbole <\"> manquant (abandon)\n"); 
				*buffer='\n'; return bufferCommandes;
			}
			*caractere = '\0'; ++caractere; continue;
		}

		if (arguments+2 == tailleMax){
			char **p = (char**) realloc(bufferCommandes, sizeof(char*)*(tailleMax+8));
			if (p == NULL){printf("Erreur de realloc (abandon)\n"); exit(55);}
			bufferCommandes = p; tailleMax += 8;
		}

		if (CARACTERE_VARIABLE(caractere)){
			bufferCommandes[arguments] = extraieLaVariable(idLocal, idGlobal, &caractere);
		}
		else if ( (buffer < caractere) && *(caractere-1) != '\\' && UN_CARACTERE_SEPARATEUR(*caractere)){
			if (CARACTERE_SEPARATEUR_TOTAL(caractere)) 
				bufferCommandes[arguments] = extraireLeSeparateur(&caractere);
			else {
				printf("erreur de syntaxe prés du symbole innatendu %c (abandon)\n", *caractere);
				*buffer = '\n';
				return bufferCommandes;
			}
		}
		else {
			char *p;
			int contientdesAntiSlash = 0;

			bufferCommandes[arguments] = caractere;
			for (; !compareDecalageAntislash(*caractere) ; ){
				if (*caractere == '\\'){
					++contientdesAntiSlash;
					if (*(caractere+1) == '\0' ||  *(caractere+1) == '\n'){
						printf("erreur de syntaxe prés du symbole innatendu <\\> (abandon)\n");
						*buffer = '\n';
						return bufferCommandes;
					} ++caractere;
				}
				++caractere;
			}
			if (*caractere == ' ' || *caractere == '\n') *caractere++ = '\0';

			//supprimer les '\'
			if (contientdesAntiSlash){
				for (p = bufferCommandes[arguments]; *p; ++p){
					if (*p == '\\'){ *(decalerDansLaMemeChaine(p+1,p)) = '\0';}				
				}
			}
		}

		for (;*caractere && isspace(*caractere); ++caractere);
	}

	bufferCommandes[arguments] = NULL;

	printf("[CONSOLE LOG] Commande lue:");afficherTableauDeString(bufferCommandes);

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
			if (p == NULL){printf("Erreur de realloc (abandon)\n"); exit(55);}
			buffer = p;
		}
	
		bufferDynamique = buffer + nombreTotalDeCaractereLue;
	
	} while (nombreDeCaractereLue == ALLOCATION_CHAINE_EXTEND_BUFFER);
	
	if (nombreTotalDeCaractereLue == tailleDuBuffer){
		p = (char*) realloc(buffer, tailleDuBuffer + 1);
		if (p == NULL){printf("Erreur de realloc (abandon)\n"); exit(55);}
		buffer = p;
	} 
	
	memset(buffer+nombreDeCaractereLue, 0, tailleDuBuffer - nombreDeCaractereLue-1);
	
	//printf("fd;%d & lu [%s] (%d caractéres), espace total de buffer %d\n", fd, buffer, nombreDeCaractereLue+1, tailleDuBuffer);
	*bufferDeSortie = buffer;
	return tailleDuBuffer;
	
}

int main(int argc, char** argv, char **envp){

	char** CommandesParLignes = NULL;
	char* bufferStdin = NULL;
	int fini = 0;
	int shellId=0;

    MemoirePartagerId idLocal, idGlobal;
	
	shellId = processPereEstUnTinyShell();
	printf("[CONSOLE LOG] open  ====== %d ====== file%d pere%d\n", shellId, getpid(), getppid());
	idLocal = creeEspaceDeMemoirePartager(genererUneClef(SHELLIDFICHIER, getpid()), 1);

	if (!shellId){
		idGlobal = creeEspaceDeMemoirePartager(genererUneClef(SHELLIDFICHIER, 1), 1);
		preformatAjouterDesValeurMemoirePartager(idGlobal, envp);

	} else idGlobal = creeEspaceDeMemoirePartager(genererUneClef(SHELLIDFICHIER, 1), 0);
	
	for (; !fini;){

		int espaceDuBuffer;
		char **bufferDeVidage;

		putchar('~'); putchar('>'); putchar(' ');
		espaceDuBuffer = bufferDepuisLentrerStandard(&bufferStdin);
		CommandesParLignes = ChaineVersTabDeChaineParReference(idLocal, idGlobal, bufferStdin);

		if (*bufferStdin == '\n' || *CommandesParLignes == NULL);
		else if ( !strcmp("exit", bufferStdin)) fini = 1;
		else if (*bufferStdin != '\n')printf("[CONSOLE LOG] :%s:\n", executeLesCommandes(CommandesParLignes)? "SUCCES" : "FAILED");

		for ( bufferDeVidage = CommandesParLignes; *bufferDeVidage ; ++bufferDeVidage){
			if (bufferStdin <= *bufferDeVidage && *bufferDeVidage <= bufferStdin + espaceDuBuffer) continue;
			else free(*bufferDeVidage);
		}

		free(CommandesParLignes);
		free(bufferStdin);
	}

	nettoieUneZoneDeMemoirePartager(idLocal);
	detruireMemoirePartager(idLocal);

	printf("[CONSOLE LOG] close ====== %d ======\n", shellId);
	if (!shellId){
		nettoieUneZoneDeMemoirePartager(idGlobal);
		detruireMemoirePartager(idGlobal);
	}

	exit(0);
}
