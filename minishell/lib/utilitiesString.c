#include "../src/quivontbien.h"

#include "CONST_mytinyshell.h"
#include "utilitiesString.h"

char* decalerDansLaMemeChaine(char *depuis, char *vers){
    
    for (; *depuis ; ++depuis, ++vers) *vers = *depuis;
    return vers;
}

char* decalerMemechaineJusquaGenerique(char *depuis, char *vers, int(*comparaison)(char c) ){

    for (; *depuis && !comparaison(*depuis); ){
        *vers = *depuis; ++vers; ++depuis;
    }

    *vers = ' ';
    return vers;
}

char *prochaineChaineApresSeparateur(char *commande, char seperateur){
    
    for (; *commande && *commande != seperateur; ++commande);
    return ++commande;
}

int comparerJusqua(char *c1, char *c2, char finc1){

    for (; *c1 && *c2 && *c1 == *c2 && *c2 != finc1; ++c1, ++c2);
    return *c1 == finc1 && (*c1 == *c2 || !*c2);

}

char* chaineCopie(char *source){

    char *dest;
    dest = (char*) malloc(sizeof(char)*(strlen(source)+1));

    strcpy(dest, source);
    return dest;
}

char* chaineCopieJusquaGenerique(char *source, int(*compare)(char c)){
    
    char *dest;
    int indice;
    
    if (source == NULL || *source == '\0') return NULL;
        
    dest = source;
    for (indice = 0; *dest && !compare(*dest); ++dest, ++indice);
    
    dest = (char*) malloc(sizeof(char)*(indice+1));
    
    memset(dest, 0, indice+1);
    strncpy(dest, source, indice);
    return dest;
}

int chaineCopieJusquaGeneriqueAvecTaille(char *source, char **dest, int(*compare)(char c)){

    *dest = chaineCopieJusquaGenerique(source, compare);
    return strlen(*dest);
}

char* chaineCopieJusqua(char *source, char seperateur){

    char *dest;
    int indice;

    if (source == NULL || *source == '\0') return NULL;
    
    dest = source;
    for (indice = 0; *dest && *dest != seperateur; ++dest, ++indice);

    dest = (char*) malloc(sizeof(char)*(indice+1));

    memset(dest, 0, indice+1);
    strncpy(dest, source, indice);
    return dest;
}

char* fusionner2(const char *mot1, const char *mot2){

    char *mot = (char*) malloc(sizeof(char)*(strlen(mot1)+strlen(mot2)+1));

    strcpy(mot, mot1);
    strcat(mot, mot2);
    strcat(mot, "\0");
    return mot;
}

char* fusionner3(const char *mot1, const char *mot2, const char *mot3){

    char *mot = (char*) malloc(sizeof(char)*(strlen(mot1)+strlen(mot2)+strlen(mot3)+2));

    strcpy(mot, mot1);
    strcat(mot, mot2);
    strcat(mot, mot3);
    strcat(mot, "\0");
    return mot;

}

char* fusionner4(const char *mot1, const char *mot2, const char *mot3, const char *mot4){
    
    char *mot = (char*) malloc(sizeof(char)*(strlen(mot1)+strlen(mot2)+strlen(mot3)+strlen(mot4)+3));
    
    strcpy(mot, mot1);
    strcat(mot, mot2);
    strcat(mot, mot3);
    strcat(mot, mot4);
    strcat(mot, "\0");
    return mot;
    
}

char **prochaineCommandeApresSeparateur(char **commande, char *seperateur){

    assert(CARACTERE_SEPARATEUR_DE_COMMANDE(seperateur));

    char **prochaineCommande;

    for (prochaineCommande = commande; *prochaineCommande && strcmp(*prochaineCommande,seperateur); ++prochaineCommande);
    if (prochaineCommande == NULL) return prochaineCommande;
    return prochaineCommande+1;
}

int compterLeNombreDeSeparateur(char **commande, char *seperateur){

    assert(CARACTERE_SEPARATEUR_DE_COMMANDE(seperateur));

    int i;
    for (i=0;*commande; ++commande) if ( !strcmp(*commande,seperateur)) ++i;
    return i;
}

char** ignoreToutLesSeparateur(char **commande, char *seperateur){

    assert(CARACTERE_SEPARATEUR_DE_COMMANDE(seperateur));

    char **prochaineCommande;

    for (prochaineCommande = commande; *prochaineCommande ; ++prochaineCommande){
        if (*prochaineCommande == NULL) return NULL;
        if (/*CARACTERE_SEPARATEUR_DE_COMMANDE(*prochaineCommande) &&*/ strcmp(*prochaineCommande, seperateur)) return prochaineCommande+1;
    }

    return NULL;
}

void ajouterLaTerminaisonALaPlaceDunEspace(char *commande){

    for (; *commande && !CARACTERE_SEPARATON_ENTRE_COMMANDE(commande); ++commande);
    if (*commande) *commande = '\0';
}

void afficherString(const char *c){

    printf("[%s]\n", c);
}

void afficherChaineJusqua(char *c, char seperateur){

    for (; *c && *c != seperateur; ++c) putchar(*c);
    putchar('\n');
}

void afficherTableauDeString(char **cc){
    
    for ( ; *cc ; ++cc) printf("[%s]", *cc);
    printf("[NULL]\n");

}