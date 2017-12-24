#include "../src/quivontbien.h"

#include "CONST_mytinyshell.h"
#include "status.h"

int executerCommandStatus(char **uneCommande){

    if ( !strcmp(*uneCommande,"status") && *(uneCommande+1) == NULL){

        if ( estNull(NOM_DERNIER_PROCESSUS) )
            printf("Aucun processus n'a était lancé\n");
        else if (CODE_DERNIERE_ARRET_OK)
            printf("%s terminé avec comme code de retour %d\n", NOM_DERNIER_PROCESSUS, CODE_DERNIERE_PROCESSUS);
        else 
            printf("%s terminé anormalement\n", NOM_DERNIER_PROCESSUS);
        
        return 1;
    } else  return IGNORE_COMMANDE;

}