#include <string.h>
#include <dirent.h>

#ifndef __MYTINISHELL_H__
#define __MYTINISHELL_H__ 1

int executeProgramme(char **uneCommande);
int executeProchaineCommande(char ***prochaineCommande, char *operateur);
int executeLesCommandes(char **pointerProchaineCommande);
char **ChaineVersTabDeChaineParReference(MemoirePartagerId idLocal, MemoirePartagerId idGlobal, char *buffer);

#endif
