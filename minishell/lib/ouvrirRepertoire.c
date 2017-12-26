#include "../src/quivontbien.h"

#include "ouvrirRepertoire.h"
#include "utilitiesString.h"

DIR* ouvrirDir(const char *path){
    
      DIR* cdir = NULL;
    
      cdir = opendir(path);
      if (cdir == NULL) printf("Erreur sur ouvrirDir avec [%s]\n", path), exit(126);
    
      return cdir;
    
 }
    
struct dirent* lireProchainFichier(DIR* cdir){

    return readdir(cdir);
}

int cheminExist(char *chemin){

    DIR* cdir = opendir(chemin);
    if (cdir == NULL){closedir(cdir); return 0;}
    closedir(cdir); return 1;

}

int fichierExist( char *fichier){

    int resultat = 0, fd;
    if ( (fd = open(fichier, O_RDONLY) != ERR)){resultat = 1; close(fd);}
    return resultat;
}

int fichierExistDans(char *repertoire, char *fichier){

    char* chemin;

    chemin = fusionner3(repertoire, "/\0", fichier);
    if (cheminExist(chemin)){ free(chemin); return 1;}
    else free(chemin); return 0;
}

int retournerLaCommandeViaPid(pid_t pid, char **resultat){

    static int NOMBRE = 12;

    int fd;
    char chainePid[NOMBRE];
    memset(chainePid, 0, NOMBRE);
    sprintf(chainePid, "%d",  pid);

    *resultat = fusionner3("/proc/", chainePid, "/cmdline");
    fd = open(*resultat, O_RDONLY);

    if (fd == -1) return 0;
    
    memset(*resultat, 0, NOMBRE);
    read(fd,*resultat, NOMBRE);
    close(fd);
    return 1;
}

int processPereEstUnTinyShell(){

    char *procPereNom = NULL;
    int resultat = 0;
    
    retournerLaCommandeViaPid(getppid(), &procPereNom);
    
    if ( !strcmp(procPereNom, "./mysh") ) resultat = 1;
    free(procPereNom);

    return resultat;

}

int obtenirLeFDFichier(char *fichierNom, int mode){

    int fd;

    fd = open(fichierNom, mode);

    if (fd == ERR){
        if (errno == ENOENT){
            fd = open(fichierNom, O_CREAT | mode, 0640);
            if (fd == ERR){
                printf(ODIR_ERREUR_OUVRIRDIRPB); return -1;
            }
        }
        else {
            printf(ODIR_ERREUR_OUVRIRDIRPB); return -1;
        }
    }

    return fd;

}