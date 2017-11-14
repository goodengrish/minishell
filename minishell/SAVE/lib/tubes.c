#include "../src/quivontbien.h"

#include "utilitiesString.h"

void executionProchaine(char **commande, int entrer[2], int sortie[2]){

    if (entrer != NULL){
        close(entrer[1]);
        close(0);
        dup(entrer[0]);
        close(entrer[0]);
    }

    if (sortie != NULL){
        close(1);
        dup(sortie[1]);
        close(sortie[1]);
        close(sortie[0]);
    }
    
    /*fprintf(stderr, "executeCommande:");
    char **c; for(c=commande;*c;++c) fprintf(stderr, "[%s]", *c);
    fprintf(stderr,"\n");*/

    execvp(*commande, commande);
    _exit(127);

}

int executerTube(int limite, char **courranteCommande, int entrer[2]){

    pid_t pid;
    int sortie[2];
    int status;

    if (!limite || courranteCommande == NULL){
        char c;
        while (read(entrer[0],&c,1)!=0) putchar(c);
        close(entrer[0]);
        return 0;
    }

    else {

        char **prochaineCommande = prochaineCommandeApresSeparateur(courranteCommande, "|\0");
        if (pipe(sortie) == ERR)  FATALE_ERREUR("sortie2-1\n",5);
        if ( (pid=fork() == ERR)) FATALE_ERREUR("fork-1\n",4);
        if (!pid){
            if (prochaineCommande != NULL) *(prochaineCommande-1) = NULL;
            executionProchaine(courranteCommande, entrer, sortie);
            _exit(127);
        }

        wait(&status);
        if (entrer != NULL) close(entrer[0]);
        close(sortie[1]);
		return (WIFEXITED(status))? ((WEXITSTATUS(status) != 0) || executerTube(limite-1,prochaineCommande,sortie)) : -1;
    }
}

int main(int argc, char **argv){

    printf("<<%d>>\n", executerTube(compterLeNombreDeSeparateur(argv+1,"|\0")+1, argv+1, NULL));

    exit(0);

}