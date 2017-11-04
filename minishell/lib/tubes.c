#include "../src/quivontbien.h"

#include "utilitiesString.h"

int executeTubes(int limite, char **commande, int tube[2]){

    pid_t pid;
    int status;
    char **prochaineCommande;
    int prochainTube[2];

    if (!limite || *commande == NULL){
        close(tube[0]); close(tube[1]); return 0;
    }

    if (pipe(prochainTube) == ERR) FATALE_ERREUR("prochainTube-1\n",6);

    prochaineCommande = prochaineCommandeApresSeparateur(commande, "|\0");
    *(prochaineCommande-1) = NULL;

    pid = fork();
    TESTFORKOK(pid);

    if (!pid){

        close(0); dup(tube[0]);
        close(1); dup(prochainTube[1]);
        close(tube[0]); 
        close(tube[1]);
        close(prochainTube[1]);
        close(prochainTube[0]);

        execvp(*commande, commande);
        exit(127);

    }

    wait(&status);
    if (WIFEXITED(status)){
        if (WEXITSTATUS(status)) return WEXITSTATUS(status);
    } else return 127;

    close(tube[0]); 
    close(tube[1]);
    close(prochainTube[1]);

    return executeTubes(limite-1, prochaineCommande, prochainTube);

}

int main(int argc, char **argv){

    int e;
    int tube[2];

    if (pipe(tube) == ERR) FATALE_ERREUR("tube-1\n",6);
    
    close(tube[1]);
    e = executeTubes(compterLeNombreDeSeparateur(argv, "|\0")+1, argv+1, tube);
    fprintf(stderr, ">>>%d<<<\n", e);

    exit(e);

}