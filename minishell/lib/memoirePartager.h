#ifndef __MEMOIRE_PARTAGER_H__
#define __MEMOIRE_PARTAGER_H__ 1

#define MEMOIRE_PARTAGER_SEPARATEUR '\n'
#define TAILLE_MEMOIRE_PARTAGER_DEFAUT 512
#define VAR_LOCAL 1
#define VAR_GLOBAL 2

typedef int MemoirePartagerId;

struct tableDeHashage {

    int semaphore_lecture;
    int semaphore_ecriture;
    char liste[TAILLE_MEMOIRE_PARTAGER_DEFAUT];
};

typedef struct tableDeHashage TableDeHachage;

// fonctions privée
int   estDansLeFormatClefValeur(char *clefvaleur);
void  detruireLesSemaphores(MemoirePartagerId id);
int   insererUneValeur(MemoirePartagerId id, char *clefvaleur);
char* obtenirLaValeur(TableDeHachage *table, char *clefvaleur);
char* obtenirLadresseDuneClefvaleur(char *chaine, char *clefvaleur);
int   supprimerClefValeur(MemoirePartagerId id, char *clefvaleur);
void  afficherUneTableDeHachage(TableDeHachage *table);

// fonctions public
MemoirePartagerId creeEspaceDeMemoirePartager(key_t clef, int creeNouveaux);
key_t genererUneClef(char *unChemin, int unEntier);
void* attacherMemoirePartager(MemoirePartagerId id);
void  detacherMemoirePartager(TableDeHachage *zone);
void  detruireMemoirePartager(MemoirePartagerId id);
void  nettoieUneZoneDeMemoirePartager(MemoirePartagerId id);
char* valeurDuneClef(char *clefvaleur);
int   obtenirLaValeurDuneClef(MemoirePartagerId id, char *clef, char **resultat);
int   preformatAjouterUneValeurMemoirePartager(MemoirePartagerId id, char *clefvaleur);
int   preformatSupprimerUneValeurMemoirePartager(MemoirePartagerId id, char *clefvaleur);
int   preformatAjouterDesValeurMemoirePartager(MemoirePartagerId id, char **ensemble);
int   executerCommandOperationSurLesVariables(int espace , char **CommandesParLignes);

#endif