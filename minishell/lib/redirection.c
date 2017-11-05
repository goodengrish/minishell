#include "../src/quivontbien.h"

#include "CONST_mytinyshell.h"
#include "ouvrirRepertoire.h"
#include "memoirePartager.h"
#include "utilitiesString.h"

/* a = cmd; b = fichier
a > b   -> overide(b,stdout)
a >> b  -> append(b,stdout)
a 2> b  -> overide(b,stderr)
a 2>> b -> append(b,stderr)
a >& b  -> overide(b, stdout & stderr)
a &>>   -> append(b, stdout & stderr)
a < b   -> stdin vers a
*/

char** pointeurProchainSeparateur(char **commande){
    
    if (commande == NULL) return NULL;
    for (; *commande && !CARACTERE_REDIRECTION(*commande); ++commande);
    return commande;
}

int executeLaCommandeAvecRedirection(char **commande, char *redirection, char *fichier){

    pid_t pid;
    int status;

    pid = fork();
	
	TESTFORKOK(pid);
	
	if (!pid){

        char **anull;
        int fd_fichier_ecraser,fd_fichier_ajouter,fd_fichier_lire;

        if ( (fd_fichier_lire = obtenirLeFDFichier(fichier, O_RDONLY)) == ERR){
            printf("Un probléme est apparut lors de l'ouverture du fichier [%s] en lecture (abandon)\n", fichier); _exit(127);
        }
        if ( (fd_fichier_ecraser = obtenirLeFDFichier(fichier, O_WRONLY)) == ERR){
            printf("Un probléme est apparut lors de l'ouverture du fichier [%s] en ecrasement (abandon)\n", fichier); _exit(127);
        }
        if ( (fd_fichier_ajouter = obtenirLeFDFichier(fichier, O_APPEND | O_WRONLY)) == ERR){
            printf("Un probléme est apparut lors de l'ouverture du fichier [%s] en ajout (abandon)\n", fichier); _exit(127);
        }

        if ( CARACTERE_REDIRECTION_O_STDOUT(redirection)){
            close(STDOUT_FILENO);dup(fd_fichier_ecraser);
        }
        else if (CARACTERE_REDIRECTION_STDIN(redirection)){
            close(STDIN_FILENO);dup(fd_fichier_lire);
        }
        else if (CARACTERE_REDIRECTION_A_STDOUT(redirection)){
            close(STDOUT_FILENO);dup(fd_fichier_ajouter);
        }
        else if (CARACTERE_REDIRECTION_O_STDOUTERR(redirection)){
            close(STDOUT_FILENO); dup(fd_fichier_ecraser); close(STDERR_FILENO); dup(fd_fichier_ecraser);
        }
        else if (CARACTERE_REDIRECTION_A_STDOUTERR(redirection)){
            close(STDOUT_FILENO); dup(fd_fichier_ajouter); close(STDERR_FILENO); dup(fd_fichier_ajouter);
        }
        else if (CARACTERE_REDIRECTION_O_STDERR(redirection)){
            close(STDERR_FILENO);dup(fd_fichier_ecraser);
        }
        else if (CARACTERE_REDIRECTION_A_STDERR(redirection)){
            close(STDERR_FILENO);dup(fd_fichier_ajouter);
        }
        else {
            printf("Erreur operateur de redirection inconnue (%s) (abandons)\n", redirection);
            _exit(127);
        }

        anull = pointeurProchainSeparateur(commande);
        *anull = NULL;

		execvp(*commande, commande);
		_exit(127);
	
	} else {
        
        wait(&status);
		return (WIFEXITED(status))? WEXITSTATUS(status) : -1;
	
	}

	return -1;

}

int executerRedirection2(char **commande, char *redirection, char *fichier){

    if (!CARACTERE_REDIRECTION(redirection)){
        printf("Erreur operateur de redirection inconnue (%s) (abandons)\n", redirection);
        return 0;
    }
    return executeLaCommandeAvecRedirection(commande, redirection, fichier);
}

int executeRedirection(char **commande){

    char **pps = pointeurProchainSeparateur(commande);
    return (*pps == NULL)? -2 : executerRedirection2(commande, *pps, *(pps+1)) == 0;

}
