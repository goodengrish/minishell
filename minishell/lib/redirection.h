#ifndef __REDIRECTION_H__
#define __REDIRECTION_H__ 1

char** pointeurProchainSeparateur(char **commande);
int executeLaCommandeAvecRedirection(char **commande, char *redirection, char *fichier);
int executerRedirection2(char **commande, char *redirection, char *fichier);
int executeRedirection(char **commande);

#endif