#ifndef __MYTINISHELL_H__
#define __MYTINISHELL_H__ 1

#define ALLOCATION_CHAINE_TAILLE_BUFFER 1024
#define ALLOCATION_CHAINE_EXTEND_BUFFER 512

#define AFFICHER_PROMPT() printf("~> ");

int executeProgramme(char **uneCommande);
int executeProchaineCommande(char ***prochaineCommande, char *operateur);
int executeLesCommandes(char **pointerProchaineCommande);
char **ChaineVersTabDeChaineParReference(MemoirePartagerId idLocal, MemoirePartagerId idGlobal, char *buffer);

#endif
