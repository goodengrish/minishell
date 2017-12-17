#include "../src/quivontbien.h"

#include "CONST_mytinyshell.h"
#include "utilitiesString.h"
#include "ouvrirRepertoire.h"
#include "utilitiesRegex.h"

#define debug 0

int comparateurEstUnCaractereRegex(char c){

    return EST_UNE_CARACTERE_REGEX(c);
}

char *placerAntiSlash(char *chaine){

  int i;
  char *c = chaine, *p;
  for (i=0; *c; ++c) if (comparateurEstUnCaractereRegex(*c)) ++i;

  c = (char*) calloc(strlen(chaine)+i+1, sizeof(char));
  for (p=c; *chaine; ++chaine, ++p){
    if ( comparateurEstUnCaractereRegex(*chaine) ){ *p = '\\'; ++p;}
    *p = *chaine;
  }

  return c;
}

int retorunePrefexDeChaine(char *argv1, char **rep, char **cregex, char **finregex){

    char *chaine = argv1, *ccregex;
    *rep = argv1; *cregex = NULL; *finregex = NULL;

    for (; *chaine ; ++chaine){
      if (*chaine == '\\') ++chaine;
      if (comparateurEstUnCaractereRegex(*chaine)) break;
    }
    if ( !(*chaine) ) return 1;

    for (; chaine != argv1; --chaine) if (*chaine == REGEX_SLASH) break;
    if (chaine == argv1) *rep = strdup(".");
    else {*chaine = REGEX_ANTISLASH_ZERO; ++chaine;}

    for (ccregex = chaine; *ccregex; ++ccregex) if (*ccregex == REGEX_SLASH) break;
    *cregex = chaine;
    if ( !(*ccregex)) return 1;

    *ccregex = REGEX_ANTISLASH_ZERO;
    *finregex = ccregex+1;

    return 0;
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

int executerRegex(char **listeRep, char ***resultat){

    DIR* dir;
    struct dirent* dirr = NULL;

    int fini = 0;
    int tailleMax = 10, indice = 0;
    char *repertoire, *cregex, *finregex;
    char **res = *resultat;

    res = (char**) calloc(tailleMax, sizeof(char*));

    for (; *listeRep ; ++listeRep){

        retorunePrefexDeChaine(*listeRep, &repertoire, &cregex, &finregex);

        if (debug) printf("Execute regex sur [%s]avec[%s] [%s]\n", repertoire, cregex, finregex);

        if (cregex == NULL) continue;
        fini = 1;

        dir = opendir(repertoire);
        if (dir == NULL) continue;

        for (; (dirr = lireProchainFichier(dir)); ){

            char *c;

            if ( *(dirr->d_name) == '.') continue;

            if (indice+2 == tailleMax){
                char **p = (char**) realloc(res, sizeof(char*)*(tailleMax+REGEX_REALLOC_EXTENTION));
                if (p == NULL) REALLOC_ERREUR(126);
                res = p; tailleMax += REGEX_REALLOC_EXTENTION;
            }

            c = placerAntiSlash(dirr->d_name);

            if ( regexValidePour(c, cregex) ){
                if (dirr->d_type == DT_DIR){
                    char *r = fusionner2(repertoire, "/");
                    res[indice++] = fusionner4(r, c, "/", finregex);
                    free(r);
                } else if (finregex == NULL || *finregex == REGEX_ANTISLASH_ZERO)
                    res[indice++] = fusionner3(repertoire, "/", c);
            }

            free(c);
        }

        closedir(dir);

    }

    res[indice] = NULL;

    *resultat = res;

    return !fini;
}

int preformatExecuterRegex(char ***bufferCommandes, int *argument, int *tailleMax, char *expresion){

    char **c;
    char **res1 = NULL, **res = NULL;
    char **commande = *bufferCommandes;
    int a = *argument, t = *tailleMax;

    if (expresion == NULL || *expresion == REGEX_ANTISLASH_ZERO) return 0;

    res = (char**) calloc(2,sizeof(char*));
    res[0] = strdup(expresion);

    for (; !executerRegex(res, &res1); ){
        free(res);
        res = res1;
        res1 = NULL;
    }

    for (c=res; *c; ++c){

       if ( t <= a+2 ){
            char **cc = (char**) realloc(commande, sizeof(char*)*(t+REGEX_REALLOC_EXTENTION));
            if (cc == NULL) REALLOC_ERREUR(126);

            memset(cc+a,0,t-a);
            commande = cc; t+=REGEX_REALLOC_EXTENTION;
        }

        commande[a++] = *c;

    } free(res);

    commande[a] = NULL;

    *bufferCommandes = commande;
    *tailleMax = t;
    *argument = a;

    return 0;

}

/*
int main(int argc, char **argv){

    int argument = 0, tailleMax = 2;
    char **buffer = (char**) calloc(tailleMax, sizeof(char*));

    if (!preformatExecuterRegex(&buffer, &argument, &tailleMax, argv[1])){

        char **c;
        for (c=buffer; *c; ++c){ printf("[%s]\n", *c); free(*c); }
        putchar('\n');
    }

    free(buffer);

    exit(0);
}
*/