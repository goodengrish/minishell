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
    
    if ( estNull(commande) ) return NULL;
    for (; *commande && !CARACTERE_REDIRECTION(*commande); ++commande);
    return commande;
}

int redirigeSiBesoin(char **commande, char *redirection, char *fichier){

    char **anull;
    int fd_fichier, doubleDup = 0;

    if ( estNull(fichier) ){
        fprintf(stderr, "Mauvaise utilisation des redirection, <cmd> <redirection> <fichier>\n");
        _exit(127);
    }

    if ( CARACTERE_REDIRECTION_O_STDOUT(redirection)){
        close(STDOUT_FILENO); fd_fichier = obtenirLeFDFichier(fichier, O_WRONLY | O_TRUNC);
    }
    else if (CARACTERE_REDIRECTION_STDIN(redirection)){
        close(STDIN_FILENO); fd_fichier = obtenirLeFDFichier(fichier, O_RDONLY);
    }
    else if (CARACTERE_REDIRECTION_A_STDOUT(redirection)){
        close(STDOUT_FILENO); fd_fichier = obtenirLeFDFichier(fichier, O_APPEND | O_WRONLY);
    }
    else if (CARACTERE_REDIRECTION_O_STDOUTERR(redirection)){
        close(STDOUT_FILENO); close(STDERR_FILENO); doubleDup=1;
        fd_fichier= obtenirLeFDFichier(fichier, O_WRONLY | O_TRUNC);
    }
    else if (CARACTERE_REDIRECTION_A_STDOUTERR(redirection)){
        close(STDOUT_FILENO); close(STDERR_FILENO); doubleDup=1;
        fd_fichier = obtenirLeFDFichier(fichier, O_APPEND | O_WRONLY);
    }
    else if (CARACTERE_REDIRECTION_O_STDERR(redirection)){
        close(STDERR_FILENO); fd_fichier = obtenirLeFDFichier(fichier, O_WRONLY | O_TRUNC);
    }
    else if (CARACTERE_REDIRECTION_A_STDERR(redirection)){
        close(STDERR_FILENO); fd_fichier = obtenirLeFDFichier(fichier, O_APPEND | O_WRONLY);
    }
    else {
        fprintf(stderr, "Erreur operateur de redirection inconnue (%s) (abandons)\n", redirection);
        _exit(127);
    }

    if ( fd_fichier == ERR){
        fprintf(stderr, "Un probléme est apparut lors de l'ouverture du fichier [%s]  (abandon)\n", fichier); _exit(127);
    }

    dup(fd_fichier);
    if (doubleDup) dup(fd_fichier);

    anull = pointeurProchainSeparateur(commande);
    *anull = NULL;

    return fd_fichier;
}

//retorune FD ou -2
int executeRedirectionSiBesoin(char **commande){

    char **pps = pointeurProchainSeparateur(commande);
    if (DEBUG) printf("Redirecteur trouver [%s]\n", *pps);
    return ( estNull(*pps) )? IGNORE_COMMANDE : redirigeSiBesoin(commande, *pps, *(pps+1));

}
