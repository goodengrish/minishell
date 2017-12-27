#ifndef __OUVRIR_REPERTOIR_H__
#define __OUVRIR_REPERTOIR_H__ 1

#include <dirent.h>
#include <sys/types.h>

#define ODIR_ERREUR_OUVRIRDIRPB "Probléme lors de l'ouverture du fichier (abandon)\n"

DIR* ouvrirDir(const char *path);
struct dirent* lireProchainFichier(DIR* cdir);
int cheminExist(char *chemin);
int fichierExist(char *fichier);
int fichierExistDans(char *repertoire, char *fichier);
void ecrireLog(char *commande);
int processPereEstUnTinyShell();
int retournerLaCommandeViaPid(pid_t pid, char **resultat);
int obtenirLeFDFichier(char *fichierNom, int);
char etatDunProcessus(pid_t pid);

#endif