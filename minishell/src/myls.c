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

#include "myls.h"  

void afficherLesAides(){

  printf("myls [OPTION] [FICHIER]\n\tAffiche les renseignements sur les fichiers donné\n\
          execute la commande sur le répertoire de base par default\n");
  printf("[OPTION]\n\t  -a  : Affiche tout les fichiers même les caché\n\t\
  -h  : Donne un nom à toutes les colones\n\t\
  -r  : affiche tout les informations en cascades (ignore les headers)\n\t\
  -R  : affiche résursivement\n\n[FICHIER]\n\t\
  --help   : affiche les aides\n\n");
}

DIR* ouvrirDir(const char *path){

  DIR* cdir = NULL;
  char *pathSlash = (char*) malloc( (strlen(path)+2) * sizeof(char));

  strcpy(pathSlash, path); strcat(pathSlash, "/\0");

  cdir = opendir(pathSlash);
  if (cdir == NULL) CUERROR(ERREUR_REPERTOIRE_NON_OUVERT, 1);

  free(pathSlash);

  return cdir;

}

struct dirent* lireProchainFichier(DIR* cdir){

  if (cdir == NULL) CUERROR(ERREUR_REPERTOIRE_NULL,2);
  return readdir(cdir);
}

void dtypeVersChar(int d_type, char** typeFichier, char **couleur){

    switch (d_type){
      case(DT_REG):   *typeFichier = MYLS_FICHIER_TYPE[2]; *couleur = EN_RESET; break;
      case(DT_DIR):   *typeFichier = MYLS_FICHIER_TYPE[0]; *couleur = EN_BLEU; break;
      case(DT_FIFO):  *typeFichier = MYLS_FICHIER_TYPE[6]; *couleur = EN_RESET; break;
      case(DT_SOCK):  *typeFichier = MYLS_FICHIER_TYPE[5]; *couleur = EN_RESET; break;
      case(DT_LNK):   *typeFichier = MYLS_FICHIER_TYPE[4]; *couleur = EN_CYAN; break;
      case(DT_BLK):   *typeFichier = MYLS_FICHIER_TYPE[3]; *couleur = EN_RESET; break;
      case(DT_CHR):   *typeFichier = MYLS_FICHIER_TYPE[1]; *couleur = EN_RESET; break;
      default: *typeFichier = NULL; *couleur = EN_RESET;
    }

}

void stmodeVersChar(char* permitions, mode_t s){

    if (S_ISDIR(s)) permitions[0] = 'd';

    if ( s & (1 << 8)) permitions[1] = 'r';
    if ( s & (1 << 7)) permitions[2] = 'w';
    if ( s & (1 << 6)) permitions[3] = 'x';

    if ( s & (1 << 5)) permitions[4] = 'r';
    if ( s & (1 << 4)) permitions[5] = 'w';
    if ( s & (1 << 3)) permitions[6] = 'x';

    if ( s & (1 << 2)) permitions[7] = 'r';
    if ( s & (1 << 1)) permitions[8] = 'w';
    if ( s & (1 << 0)) permitions[9] = 'x';

}

void printformat(char *permitions, char *proprietaire, char *groupeProprietaire, char* typefichier, int tailleOctets, const char *nomFichier, char *couleur){


    printf("%10s    %10s %10s %10s %10d %s%25s%s\n", permitions, proprietaire, groupeProprietaire, typefichier, tailleOctets, couleur, nomFichier, EN_RESET);
}

void preformat(const char* path, const struct dirent* dirr){

  char permitions[11];
  char *couleur = EN_RESET;
  char *fichierType = NULL;
  char *chemin = NULL;
  struct stat mstats;
  struct passwd* mpasswd = NULL;
  struct group* mgroup = NULL;

  chemin = (char*) malloc((strlen(path)+strlen(dirr->d_name)+1)*sizeof(char));
  strcpy(chemin, path);
  strcat(chemin, dirr->d_name);

  if (stat(chemin, &mstats) == -1) printf("Répertoire %s non reconnue ou non trouvé\n", chemin),exit(3);

  mpasswd = getpwuid(mstats.st_uid);
  if (mpasswd == NULL) CUERROR("s",4);

  mgroup = getgrgid(mstats.st_gid);
  if (mgroup == NULL) CUERROR("t",5);

  memset(&permitions, 45, 10); permitions[10] = '\0';
  stmodeVersChar(permitions, mstats.st_mode);
  dtypeVersChar(dirr->d_type, &fichierType, &couleur);
  printformat(permitions, mpasswd->pw_name ,mgroup->gr_name , fichierType, mstats.st_size ,dirr->d_name, couleur);
  free(chemin);
}

void affichageRecursif(const char *path, const Drapeaux *drap, const struct dirent *dirr){

  char nouvPath[50]; nouvPath[0] = '\0';
  strcat(nouvPath, path);
  strcat(nouvPath, dirr->d_name);
  strcat(nouvPath, "/\0");
  afficherContenu(nouvPath,drap);

}

void afficherContenu(const char *path, const Drapeaux *drap){

  DIR* cdir = ouvrirDir(path);
  struct dirent* dirr = NULL;

  //for (i = 0; i < 35; i++) putchar('-'); printf(" my  ls "); for (i = 0; i < 35; i++) putchar('-'); putchar('\n');
  if (!drap->retirer_header){
    printf("\nRépertoire: %s\n", path);
    if (drap->afficher_header) printf("PERMITIONS  PROPRIETAIRE      GROUP       TYPE     TAILLE               NOM FICHIER\n");
  }
  for (; (dirr = lireProchainFichier(cdir)) ;){
    if (dirr->d_name[0] != '.') preformat(path, dirr);
    else if (drap->autoriser_fichierCacher) preformat(path, dirr);

  }

  closedir(cdir);
  cdir = ouvrirDir(path);

  for (; (dirr = lireProchainFichier(cdir)) ;){
    if (dirr->d_name[0] != '.'  && dirr->d_type == DT_DIR && drap->autoriser_recustif) affichageRecursif(path, drap, dirr);
  }

  closedir(cdir);
}

void execute_myls(char **path, Drapeaux *drap, int argc, char **argv){

  int nombreDeRepertoireEnParam = 0;
  int i,j;
  char *s;
  char c;

  *path = REPERTOIRE_COURANT;

  memset(drap,0,sizeof(Drapeaux));

  for (i = 1; i < argc; i++){
    s = argv[i];

    if (*s == '-'){ // cmd -x

      if (s[1] != '-'){
        for (j = 1; (c=s[j]) ; j++)
          switch (c){
            case('a'): drap->autoriser_fichierCacher = 1; break;
            case('h'): drap->afficher_header = 1; break;
            case('R'): drap->autoriser_recustif = 1; break;
            case('r'): drap->retirer_header = 1; break;
            default: printf("Commande -%c non reconnue\n", c);
          }
      }
      else {
        if (!strncmp(s+2, "help", 4)) afficherLesAides(), exit(0);
        else printf("Commande %s nom reconnue\n", s);
      }
    }
  }

  for (i = 1; i < argc; i++){
    s = argv[i];
    if (*s != '-'){ afficherContenu(s, drap); nombreDeRepertoireEnParam++;}
  }

  if (!nombreDeRepertoireEnParam) afficherContenu(REPERTOIRE_COURANT, drap);
}

int main(int argc, char **argv){

  char *path = NULL;

  Drapeaux drap;
  execute_myls(&path, &drap, argc, argv);

  exit(0);
}
