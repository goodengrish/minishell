#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>

#define CACA 1

#define ERREUR_REPERTOIRE_NON_OUVERT "Le répertoire n'a pue être ouvert!\n" //Erreur 1
#define ERREUR_REPERTOIRE_NULL "le répertoire est vide\n" //Erreur 2

#define CUERROR(s,n) perror(s),exit(n)
#define EN_RESET "\x1b[0m"
#define EN_BLEU "\x1b[34m"
#define EN_CYAN "\x1b[36m"

#define REPERTOIRE_COURANT "./"

char *MYLS_FICHIER_TYPE[] = {
    "répertoire",
    "char dev",
    "fichier",
    "bloc dev",
    "lien",
    "socket",
    "FIFO"
};

typedef struct rules {
  int autoriser_fichierCacher;
  int afficher_header;
  int autoriser_recustif;
  int autoriser_details;
  int retirer_header;
} Drapeaux;



DIR* ouvrirDir(const char *path);
struct dirent* lireProchainFichier(DIR* cdir);

void afficherLesAides();
void dtypeVersChar(int d_type, char** typeFichier, char **couleur);
void stmodeVersChar(char* permitions, mode_t s);
void printformat(char *permitions, char *proprietaire, char *groupeProprietaire, char* typefichier, int tailleOctets, const char *nomFichier, char* couleur);
void preformat(const char* path, const struct dirent* dirr);
void affichageRecursif(const char *path, const Drapeaux *drap, const struct dirent *dirr);
void afficherContenu(const char *path, const Drapeaux *drap);
void execute_myls(char **path, Drapeaux *drap, int argc, char **argv);
