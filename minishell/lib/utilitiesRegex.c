#include "../src/quivontbien.h"

#include "utilitiesRegex.h"
#include "CONST_mytinyshell.h"
#include "utilitiesString.h"
#include "ouvrirRepertoire.h"

int comparateurEstUnCaractereRegex(char c){

    return EST_UNE_CARACTERE_REGEX(&c);
}

char *retorunePrefexDeChaine(char *argv1, char **postFix){

    char *chaine, *pfin;
    chaine = chaineCopieJusquaGenerique(argv1, comparateurEstUnCaractereRegex);
    pfin = chaine + (strlen(chaine));
    for (; pfin != chaine && *pfin != '/'; *pfin-- = '\0');
    
    for (pfin = argv1 + strlen(argv1); pfin != argv1 && *pfin != '/'; pfin--);
    *postFix = pfin+1;

    return chaine;
}

int regexAvecEnsemble(char *chaine, char *regex, int not){

    if ( *(regex) == ']') return not;
    if (*regex == '\0' || *(regex+1) == '\0') FATALE_ERREUR("regex inconnu '[' (abandon)\n", 99);

    if ( *(regex+1) == '-'){
        if ( *(regex+2) == '\0') FATALE_ERREUR("regex inconnu '[<caractere>-' (abandon)\n", 95);
        if ( !EST_UN_ALPHADIGIT( *(regex+2) )) FATALE_ERREUR("regex inconnu '[<caractere>-]' (abandon)\n", 94);
        if (*(regex+2) < *regex) FATALE_ERREUR("plage de regex inconnue prés de '[-]' (abandons)\n", 93);
        if (*regex < *chaine && *chaine < *(regex+2)){ 
            if (not) return 0;
            else return 1;

        } else return regexAvecEnsemble(chaine, regex+3, not);

    } else {
        if (*regex == *chaine){ 
            if (not) return 0;
            else return 1;
        } else return regexAvecEnsemble(chaine, regex+1, not);
    }

    return 0;
}

int regexValidePour(char *chaine, char *regex){

    int not = 0;

    for (; *chaine && *regex; ){

        if (*regex == '?'){
            ++chaine; ++regex; 

        } else if (*regex == '*'){
            if ( *(regex+1) == '\0') return 1;
            for (++regex ; *chaine && *chaine != *regex; ++chaine);

        } else if (*regex == '['){
            
            not = 0;
            if ( *(regex+1) == '~'){ not = 1; ++regex; }
            if ( *(regex+1) == ']') FATALE_ERREUR("regexe inconnu '[]' (abandon)\n", 98);
            ++regex;
            if (!regexAvecEnsemble(chaine,regex,not)) return 0;
            for (; *regex && *regex != ']'; ++regex);
            ++regex;
            ++chaine;

        } else {
            if (*chaine++ != *regex++) return 0;

        }
    }

    return *chaine == *regex && *regex == '\0';
}

void executerRegex(char *repertoire, char *regex){

    DIR* dir;

    if (repertoire == NULL || regex == NULL || *repertoire == '\0' || *regex == '\0') return;
    if (!strcmp(regex,"[]") || !strcmp(regex, "[~]")) return;
    
    if (repertoire == NULL || *repertoire == '\0' ) dir = ouvrirDir("./");
    else dir = ouvrirDir(repertoire);

    struct dirent* dirr = NULL;

    for (; (dirr = lireProchainFichier(dir)); ){

        if (regexValidePour(dirr->d_name, regex)){
            printf("MAtchOK [%s]\n", dirr->d_name);
        }
    }

    closedir(dir);
}

int main(int argc, char **argv){

    char *repertoire, *regex;
    repertoire = retorunePrefexDeChaine(argv[1], &regex);
    executerRegex(repertoire, regex);
    free(repertoire);
    exit(0);

    //argv[1] = regex
}

