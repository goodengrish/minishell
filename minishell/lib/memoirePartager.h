#ifndef __MEMOIRE_PARTAGER_H__
#define __MEMOIRE_PARTAGER_H__ 1

#define MEMOIRE_PARTAGER_SEPARATEUR '\n'
#define TAILLE_MEMOIRE_PARTAGER_DEFAUT 1024

#define VAR_LOCAL 1
#define VAR_GLOBAL 2

#define SET "set"
#define SETENV "setenv"
#define UNSET "unset"
#define UNSETENV "unsetenv"

#define MEMP_ERREUR_KEYT "key_t-1\n"
#define MEMP_ERREUR_SHMGET "shmget-1\n"
#define MEMP_ERREUR_SHMAT "shmat-1\n"
#define MEMP_ERREUR_PASASSEZDESPACE "Espace insufisant dans le zone pour inserer [%s] (abandons)\n"


typedef int MemoirePartagerId;

typedef struct {

    char buffer[TAILLE_MEMOIRE_PARTAGER_DEFAUT];
    int  stouage[TAILLE_MEMOIRE_PARTAGER_DEFAUT];
    int  acces;

} ZoneMp;

MemoirePartagerId creeEspaceDeMemoirePartager(key_t clef, int creeNouveaux);
key_t genererUneClef(char *unChemin, int unEntier);
void nettoieUneZoneDeMemoirePartager(MemoirePartagerId id);
void* attacherMemoirePartager(MemoirePartagerId id);
void detruireMemoirePartager(MemoirePartagerId id);
int preformatAjouterDesValeurMemoirePartager(MemoirePartagerId id, char **ensemble);
void detacherMemoirePartager(ZoneMp *zone);

int obtenirLaValeurDuneClef(MemoirePartagerId id, char *clef, char **resultat);
int executerCommandOperationSurLesVariables(int espace , char **CommandesParLignes);

#endif