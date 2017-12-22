#include "../src/quivontbien.h"

#include "utilitiesString.h"
#include "memoirePartager.h"

struct sembuf SEM_MONTER = {0,1,0};
struct sembuf SEM_DECENDRE = {0,-1,0};

void V(int s){

    semop(s, &SEM_MONTER, 1);
}

void P(int s){

    semop(s, &SEM_DECENDRE, 1);
}

int estDansLeFormatClefValeur(char *clefvaleur){

    if (*clefvaleur == EGAL) return 0;
    for ( ; *clefvaleur && *clefvaleur != EGAL; ++clefvaleur);
    if (*clefvaleur != EGAL) return 0;
    ++clefvaleur;
    return *clefvaleur;
}

key_t genererUneClef(char *unChemin, int unEntier){

    key_t k = ftok(unChemin,unEntier+1);
    if (k == ERR) FATALE_ERREUR(MEMP_ERREUR_KEYT, errno);
    return k;
}

void nettoieUneZoneDeMemoirePartager(MemoirePartagerId id){

    ZoneMp *zone = (ZoneMp*) attacherMemoirePartager(id);

    memset(zone,0,sizeof(ZoneMp));
    memset(zone->stouage, -1, TAILLE_MEMOIRE_PARTAGER_DEFAUT);
    detacherMemoirePartager(zone);
}

void detruireLesSemaphores(MemoirePartagerId id){

    ZoneMp *zone = (ZoneMp*) attacherMemoirePartager(id);
    semctl(zone->acces, 0, IPC_RMID);
    detacherMemoirePartager(zone);
}

MemoirePartagerId creeEspaceDeMemoirePartager(key_t clef, int creeNouveaux){

    MemoirePartagerId id;
    ZoneMp *table;

    if (creeNouveaux) id = shmget(clef, sizeof(ZoneMp), IPC_CREAT | 0600);
    else {
        id = shmget(clef, sizeof(ZoneMp), 0600); 
        return id; 
    }

    if (id == ERR) FATALE_ERREUR(MEMP_ERREUR_SHMGET, errno);
    nettoieUneZoneDeMemoirePartager(id);
    table = (ZoneMp*) attacherMemoirePartager(id);
    table->acces = semget(IPC_PRIVATE, 1, 0600);
    semctl(table->acces, SETVAL, 1);
    detacherMemoirePartager(table);
    return id;

}

void* attacherMemoirePartager(MemoirePartagerId id){

    void* zone = NULL;

    zone = shmat(id, 0, 0);
    if (zone == NULL) FATALE_ERREUR(MEMP_ERREUR_SHMAT, 7);
    return zone;
}

void detacherMemoirePartager(void *zone){

    shmdt(zone);
}

void detruireMemoirePartager(MemoirePartagerId id){

    detruireLesSemaphores(id);
    shmctl(id, IPC_RMID, 0);
}

int emplacementvide(char *buffer, int max){

    int i=0;
    for (; i<max && !(buffer[i]); ++i);
    return i == max;
}

int nouvelleEntrer(int *stouage, int empl){

    for (; *stouage != -1; ++stouage);
    *stouage = empl; *(stouage+1) = -1;
    return 1;
}

int sub(int *stouage, char *buffer, int bg, int bd, int tsc, char *clefval, int cvlen){

    if (tsc < cvlen+1) return 0;

    if ( (tsc>>1) < cvlen+1 ){

        if (emplacementvide(buffer+bg, tsc) ){
            nouvelleEntrer(stouage, bg);
            strncpy(buffer+bg, clefval, cvlen);
            return 1;
        }

        else if (emplacementvide(buffer+bd, tsc) ){
            nouvelleEntrer(stouage, bd);
            strncpy(buffer+bd, clefval, cvlen);
            return 1;
        }

        else return 0;
    }


    tsc >>=1;
    return sub(stouage, buffer, bg, bd-tsc, tsc, clefval, cvlen) ||
           sub(stouage, buffer, bd, bd+tsc, tsc, clefval, cvlen);
}

int obtenirp(char *buffer, int *stouage, char *clef, int cleflen, int **empl){

    *empl = NULL;
    for (; *stouage != -1; ++stouage)
        if ( !strncmp( (buffer+(*stouage) ), clef, cleflen) && buffer[(*stouage)+cleflen] == '=')
             *empl = stouage; return 1;
    
    
    return 0;
}

int supprimer(char *buffer, int *stouage, char *clef){
    
    int clen = strlen(clef);
    int *empl = NULL;

    if (obtenirp(buffer, stouage, clef, clen, &empl) && empl){

        for (buffer += *empl; *buffer; ++buffer) *buffer = '\0';
        
        for (; *(empl+1) != -1; empl++) *empl = *(empl+1);
        *empl = -1;
        return 1;
    }

    return 0;
}

int inserer(char *buffer, int *stouage, char *clefval){
    
    char *c = clefval;
    for (; *c != '='; ++c);
    *c = '\0';

    supprimer(buffer, stouage, clefval); *c = '=';
    return sub(stouage, buffer, 0, TAILLE_MEMOIRE_PARTAGER_DEFAUT>>1,
              TAILLE_MEMOIRE_PARTAGER_DEFAUT>>1, clefval, strlen(clefval));
}

int obetnir(char *buffer, int *stouage, char *clef, char **resultat){

    int clen = strlen(clef);
    int *empl = NULL;

    *resultat = NULL;
    if (obtenirp(buffer, stouage, clef, clen, &empl) && empl) *resultat = strdup(buffer+(*empl)+clen+1);
    return *resultat != NULL;
}

void afficher(char *buffer, int *stouage, void(*affichage)(char* c) ){

    for (; *stouage != -1 ; ++stouage ) affichage(buffer+(*stouage));

    putchar('\n');
    return ;
}
    
int preformatAjouterUneValeurMemoirePartager(MemoirePartagerId id, char *clefvaleur){

    int status = 0;
    
    if ( !estDansLeFormatClefValeur(clefvaleur) ) return 0;
    ZoneMp *zone = (ZoneMp*) attacherMemoirePartager(id);
    P(zone->acces);
    status = inserer(zone->buffer,zone->stouage, clefvaleur);
    V(zone->acces);
    detacherMemoirePartager(zone);
    return status;
    
}

int preformatSupprimerUneValeurMemoirePartager(MemoirePartagerId id, char *clefvaleur){
    
    int status = 0;
    
    ZoneMp *zone = (ZoneMp*) attacherMemoirePartager(id);
    P(zone->acces);
    status = supprimer(zone->buffer,zone->stouage, clefvaleur);
    V(zone->acces);
    detacherMemoirePartager(zone);
    return status;
}

int preformatAjouterDesValeurMemoirePartager(MemoirePartagerId id, char **ensemble){

    for (; *ensemble && preformatAjouterUneValeurMemoirePartager(id, *ensemble); ++ensemble);
    return 1;
}

int preformatAfficherMemoirePartager(MemoirePartagerId id, void(*affichage)(char* c) ){

    ZoneMp *zone = (ZoneMp*) attacherMemoirePartager(id);
    P(zone->acces);
    afficher(zone->buffer,zone->stouage,affichage);
    V(zone->acces); 
    detacherMemoirePartager(zone);
    return 1;

}

int obtenirLaValeurDuneClef(MemoirePartagerId id, char *clef, char **resultat){
    
    int status = 0;
    
    ZoneMp *zone = (ZoneMp*) attacherMemoirePartager(id);
    P(zone->acces);
    status = obetnir(zone->buffer,zone->stouage, clef, resultat);
    V(zone->acces);
    detacherMemoirePartager(zone);
    return status;
    
}

int executerCommandOperationSurLesVariables(int espace , char **CommandesParLignes){
    
    int res = IGNORE_COMMANDE;
    char *ajouter, *enlever;
    MemoirePartagerId id;

    if (espace == VAR_LOCAL){ 
        ajouter = SET; enlever = UNSET;
        id = creeEspaceDeMemoirePartager(genererUneClef(SHELLIDFICHIER, getpid()), 0);
    }
    else if (espace == VAR_GLOBAL){ 
        ajouter = SETENV; enlever = UNSETENV;
        id = creeEspaceDeMemoirePartager(genererUneClef(SHELLIDFICHIER, 1), 0);
    }
    else return res;
    
    if (**CommandesParLignes == '\n') return 0;
    if (!strcmp(ajouter, *CommandesParLignes)){
    
        if (*(CommandesParLignes+1) == NULL){
            preformatAfficherMemoirePartager(id, afficherString);
            return 1;
        }
    
        res = preformatAjouterUneValeurMemoirePartager(id,*(CommandesParLignes+1));
    
        switch (res){
            case(0): printf("Commande incompléte (utiliser %s <variable>=<valeur>)\n", ajouter); break;
            case(-1): printf("la variable doit être initialisé  (utiliser %s <variable>=<valeur>)\n", ajouter); break;
        }
    }
    else if (!strcmp(enlever, *CommandesParLignes)){

        if (*(CommandesParLignes+1) == NULL){
            printf("Commande incompléte (utiliser %s <variable>)\n", enlever); return 0;
        }
    
        res = preformatSupprimerUneValeurMemoirePartager(id,*(CommandesParLignes+1));
        switch (res){
            case(0): printf("Element non trouvé\n"); break;
            case(-1): printf("la variable n'existe pas (abandons)\n"); break;
        }
    }
    
    return res;
        
}
