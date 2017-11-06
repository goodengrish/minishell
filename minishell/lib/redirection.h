#ifndef __REDIRECTION_H__
#define __REDIRECTION_H__ 1

char** pointeurProchainSeparateur(char **commande);
int redirigeSiBesoin(char **commande, char *redirection, char *fichier);
int executeRedirectionSiBesoin(char **commande);

#endif