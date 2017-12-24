#include "../src/quivontbien.h"

#include <glob.h>

#include "CONST_mytinyshell.h"
#include "utilitiesString.h"
#include "ouvrirRepertoire.h"
#include "utilitiesRegex.h"

int comparateurEstUnCaractereRegex(char c){

    return EST_UNE_CARACTERE_REGEX(c);
}

int leGlobErreur(const char *chemin, int erreur){

    fprintf(stderr, "Impossible de lire le répértoire %s, il y a probablement une \
    erreur de droits (read) (répértoire ignoré)\n", chemin);
    fprintf(stderr, "Code de retoure après tentative d'ouverture de répértoire %d\n", erreur);

    return 0;
}

int preformatExecuterRegex(char ***bufferCommandes, int *argument, int *tailleMax, char *expresion){

    int elementLu = 0;
    glob_t resultat;
    memset(&resultat, 0, sizeof(glob_t));

   if ( !glob(expresion, GLOB_ERR, leGlobErreur, &resultat) ){

       char **liste = resultat.gl_pathv;

       if ( resultat.gl_pathc >= (*tailleMax)-(*argument) ){
         char **p = (char**) realloc(*bufferCommandes, sizeof(char*)*((*tailleMax)+resultat.gl_pathc ));
         if ( estNull(p) ) REALLOC_ERREUR(126);
         memset(p+(*argument), 0, resultat.gl_pathc);
         *bufferCommandes = p; *tailleMax += resultat.gl_pathc;
       }

       for (; *liste && elementLu < resultat.gl_pathc; ++liste){

           elementLu++;
           (*bufferCommandes)[(*argument)++] = strdup(*liste);
       }
    }

    if ( !elementLu ) (*bufferCommandes)[*argument] = expresion;
    else (*argument)--;

    globfree(&resultat);
    return 0;

}