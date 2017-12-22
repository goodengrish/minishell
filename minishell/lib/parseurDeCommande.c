#include "../src/quivontbien.h"

#include "CONST_mytinyshell.h"
#include "redirection.h"
#include "memoirePartager.h"
#include "parseurDeCommande.h"
#include "utilitiesString.h"
#include "utilitiesRegex.h"

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

char *extraireUneChaineQuote(char **caractere){

	char *chaineFormater = *caractere;
	char *c = *caractere;

	for (++c; *c && *c != QUOTE; ++c);
	if (*c == ANTISLASHZERO) return NULL;
	
	//*c = ANTISLASHZERO;
	*caractere = c+1;
	return chaineFormater;

}

char **auto_realloc(char **bufferCommandes ,int *tailleMax){

	static const int PDC_REALLOCATION = 8;

	char **p = (char**) realloc(bufferCommandes, sizeof(char*)*( (*tailleMax) +PDC_REALLOCATION) );
	if ( estNull(p) ) REALLOC_ERREUR(126);
	(*tailleMax) += PDC_REALLOCATION;
	return p;
}
    
int compareDecalageAntislash(char caractere){    
    return caractere == ANTISLASHZERO || caractere == ESPACE || caractere == RETOURALALIGNE || UN_CARACTERE_SEPARATEUR(caractere);
}

char **ChaineVersTabDeChaineParReference(MemoirePartagerId idLocal, MemoirePartagerId idGlobal, char *buffer){
    // ne pas free(buffer) sous paine d'incohérence des elements de sortie

    char **bufferCommandes = (char**) malloc(sizeof(char*)*TAILLE_SORTIE_DEFAUT);
	char *caractere;
	
	int arguments = 0, tailleMax = TAILLE_SORTIE_DEFAUT;
	int carPrecedantEstSeparateur = 0;

	memset(bufferCommandes,0,TAILLE_SORTIE_DEFAUT);
	if (UN_CARACTERE_SEPARATEUR(*buffer)) PDC_ERREUR_EXECUTER_ERREUR_SYMBOLE_INNATENDU(*buffer, buffer, bufferCommandes);

	for (caractere = buffer ; *caractere && isspace(*caractere); ++caractere);
	for (arguments = 0 ; *caractere ; ++arguments){

		if (arguments+2 == tailleMax)
			bufferCommandes = auto_realloc(bufferCommandes, &tailleMax);

		if (*caractere == RETOURALALIGNE){*caractere = ANTISLASHZERO;continue;}
		
		if (*caractere == POINTVIRGULE ){
			*caractere = ANTISLASHZERO;
			bufferCommandes[arguments] = chaineCopie(";\0");
			++caractere;
			carPrecedantEstSeparateur = 0;

		}
		else if (*caractere == QUOTE){
			bufferCommandes[arguments] = extraireUneChaineQuote(&caractere);
			if ( estNull(bufferCommandes[arguments]) )
				PDC_ERREUR_EXECUTE_ANTISLASH_VIDE(buffer,bufferCommandes);
			carPrecedantEstSeparateur = 0;

		}
		else if (CARACTERE_VARIABLE(caractere)){
			bufferCommandes[arguments] = extraieLaVariable(idLocal, idGlobal, &caractere);
			carPrecedantEstSeparateur = 0;

		}
		else if ( (buffer < caractere) && *(caractere-1) != ANTISLASH && UN_CARACTERE_SEPARATEUR(*caractere)){

			if (CARACTERE_SEPARATEUR_TOTAL(caractere)){

				if (carPrecedantEstSeparateur) PDC_ERREUR_EXECUTER_ERREUR_SYMBOLE_INNATENDU(*caractere,buffer,bufferCommandes);

				bufferCommandes[arguments] = extraireLeSeparateur(&caractere);
				carPrecedantEstSeparateur = 1;
			}
			else PDC_ERREUR_EXECUTER_ERREUR_SYMBOLE_INNATENDU(*caractere, buffer, bufferCommandes);
			
		}
		else {
			int contientDesRegex = 0;

			bufferCommandes[arguments] = caractere;
			for (; !compareDecalageAntislash(*caractere) ; ){

				if ( EST_UNE_CARACTERE_REGEX(*(caractere+1)) ) ++contientDesRegex;

				if ( CARACTERE_VARIABLE(caractere)){
					char *s = caractere;
					char *var = extraieLaVariable(idLocal, idGlobal, &caractere);
					*s = ANTISLASHZERO;

					bufferCommandes[arguments] = fusionner2(bufferCommandes[arguments], var);
					free(var);
					
				}
				++caractere;
			}

			if (*caractere == ESPACE || *caractere == RETOURALALIGNE) *caractere++ = ANTISLASHZERO;

			if(contientDesRegex)
			    preformatExecuterRegex(&bufferCommandes, &arguments, &tailleMax, bufferCommandes[arguments]);

			carPrecedantEstSeparateur = 0;
		}

		for (;*caractere && isspace(*caractere); ++caractere);
	}

	bufferCommandes[arguments] = NULL;

	if (DEBUG){printf("[CONSOLE LOG] Commande lue:");afficherTableauDeString(bufferCommandes);}

	return bufferCommandes;

}