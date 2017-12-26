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

#define MP_CREE(clef, s) (shmget(clef, s, IPC_CREAT | 0600))
#define MP_MAT(r,t,id) (r=(t)shmat(id,0,0))

#define shmPPEobtenir(var,id,val,type) {MP_MAT(val,type,id);\
    var = *val;\
    shmdt((char*)val); }
#define shmPPEChanger(var,id,val,type) {MP_MAT(val,type,id);\
    *val = var;\
    shmdt((char*)val); }

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
int preformatAfficherMemoirePartager(MemoirePartagerId id, void(*affichage)(char* c) );
int preformatAjouterUneValeurMemoirePartager(MemoirePartagerId id, char *clefvaleur);
int preformatSupprimerUneValeurMemoirePartager(MemoirePartagerId id, char *clefvaleur);
int preformatAjouterDesValeurMemoirePartager(MemoirePartagerId id, char **ensemble);
void detacherMemoirePartager(void *zone);

int obtenirLaValeurDuneClef(MemoirePartagerId id, char *clef, char **resultat);
int executerCommandOperationSurLesVariables(int espace , char **CommandesParLignes);

#endif