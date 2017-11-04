#ifndef __OUVRIR_REPERTOIR_H__
#define __OUVRIR_REPERTOIR_H__ 1

#include <dirent.h>
#include <sys/types.h>

DIR* ouvrirDir(const char *path);
struct dirent* lireProchainFichier(DIR* cdir);
int cheminExist(char *chemin);
int fichierExistDans(char *repertoire, char *fichier);
void ecrireLog(char *commande);
int processPereEstUnTinyShell();

#endif