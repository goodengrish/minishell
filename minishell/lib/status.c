#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "CONST_mytinyshell.h"

int executerCommandStatus(char **uneCommande){

    if ( !strcmp(*uneCommande,"status") && *(uneCommande+1) == NULL){

        if (CODE_DERNIERE_ARRET_OK)
            printf("%s terminé avec comme code de retour %d\n", NOM_DERNIER_PROCESSUS, CODE_DERNIERE_PROCESSUS);
        else 
            printf("%s terminé annormalement\n", NOM_DERNIER_PROCESSUS);
        
        return 1;
    } else  return -2;

}