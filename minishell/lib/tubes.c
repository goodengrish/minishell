#include "../src/quivontbien.h"

#include "utilitiesString.h"

int executerTube(char **commande, int *entrer){

    pid_t pid;
    int sortie[2];

    if ( estNull(commande) ){
        char c;
        for (; read(entrer[0], &c, 1); putchar(c));
        close(entrer[0]);
        return 1;
    }

    char **prochaineCommande = prochaineCommandeApresSeparateur(commande, "|");

    if ( nonNull(prochaineCommande) ) *(prochaineCommande-1) = NULL;

    if ( pipe(sortie) == ERR ) FATALE_ERREUR("pipe -1\n", 126);

    pid = fork();
    if (pid == ERR) FATALE_ERREUR("fork-1\n", 127);

    if (!pid){

        if ( nonNull(entrer) ) close(0),dup(entrer[0]),close(entrer[0]);
    
        close(1); dup(sortie[1]);
        close(sortie[1]);

        execvp(*commande, commande);
    }

    wait(NULL);
    if ( nonNull(entrer) ) close(entrer[0]);
    close(sortie[1]);
    
    return executerTube(prochaineCommande, sortie);

}

int main(int argc, char **argv){

    printf("\n[[%d]]\n",executerTube(argv+1, NULL));

    exit(0);
}