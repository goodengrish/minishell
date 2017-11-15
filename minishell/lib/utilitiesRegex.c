#include "../src/quivontbien.h"

#include "CONST_mytinyshell.h"
#include "utilitiesString.h"
#include "ouvrirRepertoire.h"
#include "utilitiesRegex.h"

int comparateurEstUnCaractereRegex(char c){

    return EST_UNE_CARACTERE_REGEX(c);
}

char *retorunePrefexDeChaine(char *argv1, char **postFix){

    char *chaine, *pfin;

    if (!chaineContientUnCaractere(argv1, REGEX_SLASH)){
        *postFix = argv1;
        return NULL;
    }

    chaine = chaineCopieJusquaGenerique(argv1, comparateurEstUnCaractereRegex);
    pfin = chaine + (strlen(chaine));
    for (; pfin != chaine && *pfin != REGEX_SLASH; *pfin-- = REGEX_ANTISLASH_ZERO);
    
    for (pfin = argv1 + strlen(argv1); pfin != argv1 && *pfin != REGEX_SLASH; pfin--);
    if (*pfin == REGEX_SLASH) ++pfin;
    *postFix = pfin;

    return chaine;
}

int regexAvecEnsemble(char *chaine, char *regex, int not){

    if ( *(regex) == REGEX_CROCHET_FERMANT) return not;
    if (*regex == REGEX_ANTISLASH_ZERO || *(regex+1) == REGEX_ANTISLASH_ZERO) RETURN_ERREUR(REGEX_ERREUR_INCONNUE, ERR);

    if ( *(regex+1) == REGEX_INTERVAL){
        if ( *(regex+2) == REGEX_ANTISLASH_ZERO) RETURN_ERREUR(REGEX_ERREUR_INCOMPLETE, ERR);
        if ( !EST_UN_ALPHADIGIT( *(regex+2) )) RETURN_ERREUR(REGEX_ERREUR_INCOMPLETE, ERR);
        if (*(regex+2) < *regex) RETURN_ERREUR(REGEX_ERREUR_PLAGE_ANBIGUE, ERR);
        if (*regex < *chaine && *chaine < *(regex+2)) return !not;
        else return regexAvecEnsemble(chaine, regex+3, not);

    } else {
        if (*regex == *chaine)return !not;
        else return regexAvecEnsemble(chaine, regex+1, not);
    }

    return 0;
}

int regexValidePour(char *chaine, char *regex){

    int not = 0, status;

    for (; *chaine && *regex; ){

        if (*regex == REGEX_QUESTION_MARK){
            ++chaine; ++regex; 

        } else if (*regex == REGEX_ETOILE_MARK){
            if ( *(regex+1) == REGEX_ANTISLASH_ZERO) return 1;
            for (++regex ; *chaine && *chaine != *regex; ++chaine);

        } else if (*regex == REGEX_CROCHET_OUVRANT){
            not = 0; ++regex;
            if ( *regex == REGEX_ANTISLASH_ZERO) RETURN_ERREUR(REGEX_ERREUR_INCOMPLETE, ERR);
            if ( *regex == REGEX_TILD){ not = 1; ++regex; }
            if ( *regex == REGEX_CROCHET_FERMANT) RETURN_ERREUR(REGEX_ERREUR_PLAGE_VIDE, ERR);

            status = regexAvecEnsemble(chaine,regex,not);
            if (status == ERR) return ERR;
            if (!status) return 0;

            for (; *regex && *regex != REGEX_CROCHET_FERMANT; ++regex);
            ++regex;
            ++chaine;

        } else {
            if (*chaine++ != *regex++) return 0;
        }
    }

    return *chaine == *regex && *regex == REGEX_ANTISLASH_ZERO;
}

int executerRegex(char ***bufferCommandes, int *argument, int *tailleMax, char *repertoire, char *regex){

    DIR* dir;
    struct dirent* dirr = NULL;
    int args = *argument, tailleMa = *tailleMax;
    int status;

    dir = ouvrirDir(repertoire);

    for (; (dirr = lireProchainFichier(dir)); ){

        if ( *(dirr->d_name) == '.') continue;

        if (tailleMa-2 < args){
			char **p = (char**) realloc(*bufferCommandes, sizeof(char*)*(tailleMa+REGEX_REALLOC_EXTENTION));
			if (p == NULL) REALLOC_ERREUR(126);
            *bufferCommandes = p; tailleMa += REGEX_REALLOC_EXTENTION;
		}

        if ( (status=regexValidePour(dirr->d_name, regex))){
            if (status == ERR) return ERR;
            (*bufferCommandes)[args++] = fusionner2(repertoire,dirr->d_name);
        }
    }

    *argument = args; *tailleMax = tailleMa;
    (*bufferCommandes)[args] = NULL;

    free(repertoire);
    closedir(dir);
    return 0;
}

int preformatExecuterRegex(char ***bufferCommandes, int *argument, int *tailleMax, char *expresion){
    
    char *repertoire, *regex;
    if (expresion == NULL || *expresion == REGEX_ANTISLASH_ZERO) return 0;
    repertoire = retorunePrefexDeChaine(expresion, &regex);
    
    if (regex == NULL || *regex == REGEX_ANTISLASH_ZERO) return 0;
    if (repertoire == NULL || *repertoire == REGEX_ANTISLASH_ZERO ){
        free(repertoire);
        repertoire = fusionner2(getenv("PWD"), "/");
    }
    
    printf("Execute regex sur [%s]avec[%s]\n", repertoire, regex);
    return executerRegex(bufferCommandes,argument,tailleMax, repertoire, regex);
}
