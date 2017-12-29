#ifndef __MYTINISHELL_H__
#define __MYTINISHELL_H__ 1

#define ALLOCATION_CHAINE_TAILLE_BUFFER 1024
#define ALLOCATION_CHAINE_EXTEND_BUFFER 512

#define MON_SININT_YES 'y'
#define MON_SININT_NO  'n'
#define EXIT_STR "exit"
#define CD_STR "cd"
#define HOME_STR "/home"

#define PID_PROCESS_EXCEV_CLEF "/bin/ls"

#define AFFICHER_PROMPT() printf("~> ");

void changerPidExec(int pid);
void monSigTstp();
void monSigInt();

int executeProgramme(char **uneCommande, int*);
int executeProchaineCommande(char ***prochaineCommande, char *operateur);
int executeLesCommandes(char **pointerProchaineCommande);
char **ChaineVersTabDeChaineParReference(MemoirePartagerId idLocal, MemoirePartagerId idGlobal, char *buffer);

#endif
