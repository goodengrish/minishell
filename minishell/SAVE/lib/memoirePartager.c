#include "../src/quivontbien.h"

#include "utilitiesString.h"
#include "memoirePartager.h"

struct sembuf SEM_MONTER = {0,1,0};
struct sembuf SEM_DECENDRE = {0,-1,0};

int estDansLeFormatClefValeur(char *clefvaleur){

    if (*clefvaleur == '=') return 0;
    for ( ; *clefvaleur && *clefvaleur != '='; ++clefvaleur);
    if (*clefvaleur != '=') return 0;
    ++clefvaleur;
    return *clefvaleur != '\0';
}

key_t genererUneClef(char *unChemin, int unEntier){

    key_t k = ftok(unChemin,unEntier+1);
    if (k == ERR) FATALE_ERREUR("key_t-1\n", errno);
    return k;
}

void nettoieUneZoneDeMemoirePartager(MemoirePartagerId id){

    TableDeHachage *zone = (TableDeHachage*) attacherMemoirePartager(id);

    memset(zone,0,sizeof(TableDeHachage));
    detacherMemoirePartager(zone);
}

void detruireLesSemaphores(MemoirePartagerId id){

    TableDeHachage *zone = (TableDeHachage*) attacherMemoirePartager(id);
    semctl(zone->semaphore_ecriture, 0, IPC_RMID);
    semctl(zone->semaphore_lecture , 0, IPC_RMID);
    detacherMemoirePartager(zone);
}

MemoirePartagerId creeEspaceDeMemoirePartager(key_t clef, int creeNouveaux){

    MemoirePartagerId id;
    TableDeHachage *table;

    if (creeNouveaux) id = shmget(clef, sizeof(TableDeHachage), IPC_CREAT | 0600);
    else {
        id = shmget(clef, sizeof(TableDeHachage), 0600); 
        return id; 
    }

    if (id == ERR) FATALE_ERREUR("shmget-1\n", errno);
    nettoieUneZoneDeMemoirePartager(id);
    table = (TableDeHachage*) attacherMemoirePartager(id);
    table->semaphore_lecture = semget(IPC_PRIVATE, 1, 0600);
    table->semaphore_ecriture =semget(IPC_PRIVATE, 1, 0600);
    semctl(table->semaphore_ecriture, SETVAL, 1);
    semctl(table->semaphore_lecture , SETVAL, 1);
    detacherMemoirePartager(table);
    return id;

}

void* attacherMemoirePartager(MemoirePartagerId id){

    void* zone = NULL;

    zone = shmat(id, 0, 0);
    if (zone == NULL) FATALE_ERREUR("shmat-1\n", 7);
    return zone;
}

void detacherMemoirePartager(TableDeHachage *zone){

    shmdt(zone);
}

void detruireMemoirePartager(MemoirePartagerId id){

    detruireLesSemaphores(id);
    shmctl(id, IPC_RMID, 0);
}

int insererUneValeur(MemoirePartagerId id, char *clefvaleur){
    
    char *sauvegarde, *lecteur;
    int resultat = 0;
    TableDeHachage *table;
    
    table = (TableDeHachage*) attacherMemoirePartager(id);
    semop(table->semaphore_ecriture, &SEM_DECENDRE, 1);
        
    lecteur = obtenirLadresseDuneClefvaleur(table->liste, clefvaleur);

    sauvegarde = NULL;
    if (lecteur != NULL){
        char *plecteur = prochaineChaineApresSeparateur(lecteur, MEMOIRE_PARTAGER_SEPARATEUR);
        sauvegarde = chaineCopieJusqua(lecteur, MEMOIRE_PARTAGER_SEPARATEUR);
        lecteur = decalerDansLaMemeChaine(plecteur, lecteur);

    }

    if (TAILLE_MEMOIRE_PARTAGER_DEFAUT -1 <= (lecteur - table->liste) + strlen(clefvaleur)){
        printf("Espace insufisant dans le zone pour inserer [%s] (abandons)\n", clefvaleur);

        if (sauvegarde != NULL) {
            strcpy(lecteur, sauvegarde);
            strcat(lecteur, "\n");
        }
        
    }
    else {
        strcpy(lecteur, clefvaleur);
        strcat(lecteur, "\n");
        resultat = 1;
    }
    
    free(sauvegarde);
    
    semop(table->semaphore_ecriture, &SEM_MONTER, 1);
    detacherMemoirePartager(table);
    
    return resultat;
}

char* valeurDuneClef(char *clefvaleur){

    for (; *clefvaleur && *clefvaleur != '='; ++clefvaleur);
    return ++clefvaleur;
}

char *obtenirLadresseDuneClefvaleur(char *chaine, char *clefvaleur){

    for (; *chaine ; chaine = prochaineChaineApresSeparateur(chaine, MEMOIRE_PARTAGER_SEPARATEUR)){
        if ( comparerJusqua(chaine,clefvaleur, '=') ) return chaine;
    }

    return chaine;
}

char* obtenirLaValeur(TableDeHachage *table, char *clefvaleur){
    
    char *p = obtenirLadresseDuneClefvaleur(table->liste, clefvaleur);
    if (*p == '\0') return NULL;
    return chaineCopieJusqua(valeurDuneClef(p), MEMOIRE_PARTAGER_SEPARATEUR);
}

int supprimerClefValeur(MemoirePartagerId id, char *clefvaleur){

    char *lecteur;
    int resultat = 0;
    TableDeHachage *table;
    
    table = (TableDeHachage*) attacherMemoirePartager(id);
    semop(table->semaphore_ecriture, &SEM_DECENDRE, 1);
        
    lecteur = obtenirLadresseDuneClefvaleur(table->liste, clefvaleur);
    if (lecteur != NULL){
        char *plecteur = prochaineChaineApresSeparateur(lecteur, MEMOIRE_PARTAGER_SEPARATEUR);
        lecteur = decalerDansLaMemeChaine(plecteur, lecteur);
        *lecteur = '\0';
        resultat = 1;
    }

    semop(table->semaphore_ecriture, &SEM_MONTER, 1);
    detacherMemoirePartager(table);
    return resultat;
}

void afficherUneTableDeHachage(TableDeHachage *table){

    printf("[%s]", table->liste);
    printf("\n");
}
    
int preformatAjouterUneValeurMemoirePartager(MemoirePartagerId id, char *clefvaleur){
    
    if ( !estDansLeFormatClefValeur(clefvaleur) ) return 0;
    return insererUneValeur(id,clefvaleur);
    
}

int preformatSupprimerUneValeurMemoirePartager(MemoirePartagerId id, char *clefvaleur){
    
    if (clefvaleur == NULL) return 0;
    return supprimerClefValeur(id,clefvaleur);
}

int preformatAjouterDesValeurMemoirePartager(MemoirePartagerId id, char **ensemble){

    for (; *ensemble && preformatAjouterUneValeurMemoirePartager(id, *ensemble); ++ensemble);
    return 1;
}

int preformatAfficherMemoirePartager(MemoirePartagerId id){

    TableDeHachage *table;
    table = (TableDeHachage*) attacherMemoirePartager(id);
    afficherUneTableDeHachage(table);
    detacherMemoirePartager(table);
    return 1;

}

// par copy  RET 0 SI non trouvé, 1 si trouvé
int obtenirLaValeurDuneClef(MemoirePartagerId id, char *clef, char **resultat){
    
    TableDeHachage *table;
    
    *resultat = NULL;
    table = (TableDeHachage*) attacherMemoirePartager(id);
    *resultat = obtenirLaValeur(table, clef);
    detacherMemoirePartager(table);
    return (*resultat == NULL)? 0 : 1;
    
}

// non générique enfin si on quitte les set/setenv
int executerCommandOperationSurLesVariables(int espace , char **CommandesParLignes){
    
    int res = -2;
    char *ajouter, *enlever;
    MemoirePartagerId id;

    if (espace == VAR_LOCAL){ 
        ajouter = "set"; enlever = "unset";
        id = creeEspaceDeMemoirePartager(genererUneClef(SHELLIDFICHIER, getpid()), 0);
    }
    else if (espace == VAR_GLOBAL){ 
        ajouter = "setenv"; enlever = "unsetenv";
        id = creeEspaceDeMemoirePartager(genererUneClef(SHELLIDFICHIER, 1), 0);
    }
    else return res;
    
    if (**CommandesParLignes == '\n') return 0;
    if (!strcmp(ajouter, *CommandesParLignes)){
    
        if (*(CommandesParLignes+1) == NULL){
            preformatAfficherMemoirePartager(id);
            return 1;
        }
    
        res = preformatAjouterUneValeurMemoirePartager(id,*(CommandesParLignes+1));
    
        switch (res){
            case(0): printf("Commande incompléte (utiliser %s <variable>=<valeur>)\n", ajouter); break;
            case(-1): printf("la variable doit être initialisé  (utiliser %s <variable>=<valeur>)\n", ajouter); break;
        }
    }
    else if (!strcmp(enlever, *CommandesParLignes)){
    
        res = preformatSupprimerUneValeurMemoirePartager(id,*(CommandesParLignes+1));
        switch (res){
            case(0): printf("Commande incompléte (utiliser %s <variable>)\n", enlever); break;
            case(-1): printf("la variable n'existe pas (abandons)\n"); break;
        }
    }
    
    return res;
        
}
