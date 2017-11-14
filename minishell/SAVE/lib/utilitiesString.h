#ifndef __UTILITES_H__
#define __UTILITES_H__ 1

#define TAILLE_SORTIE_DEFAUT 32
#define TAILLE_SORTIE_EXTEND 16

int chaineContientUnCaractere(char *chaine, char car);
int chaineContientUnCaractereGenerique(char *chaine, int(*comparaison)(char c));

/** ALLOUE et retourne une copie de source **/
char* chaineCopie(char *source);
char* chaineCopieJusqua(char *source, char seperateur);
char* chaineCopieJusquaGenerique(char *source, int(*compare)(char c));
int chaineCopieJusquaGeneriqueAvecTaille(char *source, char **dest, int(*compare)(char c)); // retorune la taille

/** ALLOUE et retourne une chaine contenant tout les chaines dans l'ordre passé en arguments **/
char* fusionner2(const char *mot1, const char *mot2);
char* fusionner3(const char *mot1, const char *mot2, const char *mot3);
char* fusionner4(const char *mot1, const char *mot2, const char *mot3, const char *mot4);

/** afficheur **/
void afficherString(const char *c);
void afficherChaineJusqua(char *c, char seperateur);
void afficherTableauDeString(char **cc);

/** Decalage **/
char* decalerDansLaMemeChaine(char *depuis, char *vers);
char* decalerMemechaineJusquaGenerique(char *depuis, char *vers, int(*compare)(char c) );


/* compare c1 avec c2 et s'arréte quand c1[i] = finc1 
 * return 1 si c1 = c2 , 0 sinon*/
int comparerJusqua(char *c1, char *c2, char finc1);

/** retourne le pointer vers nouvelle zone ayant eu comme precedents caractére la chaine separateur 
 * ("aaa && bb || cc", "||"") -> cc
**/
char **prochaineCommandeApresSeparateur(char **commande, char *seperateur);

/** retourne le pointer vers nouvelle zone ayant eu comme precedents caractére la chaine separateur 
 * ("aaa&&bb|cc","|") -> cc
**/
char *prochaineChaineApresSeparateur(char *chaine, char separateur);

/** retourne le nombre de chaine etait agal au seprarateur */
int compterLeNombreDeSeparateur(char **commande, char *seperateur);

/** retorune le pointer vers le caractére se situant aprés un séparateur différent du separateur passé
 *  en argument
 *  ("a || b || c && d", "||") -> d
**/
char **ignoreToutLesSeparateur(char **commande, char *seperateur);

/** "abcd efgh" -> abcd\0efgh **/
void ajouterLaTerminaisonALaPlaceDunEspace(char *commande);


#endif