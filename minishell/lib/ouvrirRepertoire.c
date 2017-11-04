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

int fichierExistDans(char *repertoire, char *fichier){

    char* chemin;

    chemin = fusionner3(repertoire, "/\0", fichier);
    if (cheminExist(chemin)){ free(chemin); return 1;}
    else free(chemin); return 0;
}

void ecrireLog(char *commande){

    int fd = open("log.txt", O_WRONLY);
    if (fd == -1){printf("erreur de fd dans ecireLog\n"); exit(5);}
    write(fd, commande, strlen(commande));
    close(fd);
}

int processPereEstUnTinyShell(){

    static int NOMBRE = 12;
    char chainePid[NOMBRE];
    char procPereNom[NOMBRE];
    char *procPereChemin;
    int resultat = 0;
    int fd;
    
    memset(chainePid, 0, NOMBRE);
    sprintf(chainePid, "%d",  getppid());

    procPereChemin = fusionner3("/proc/", chainePid, "/cmdline");
    fd = open(procPereChemin, O_RDONLY);

    if (fd == -1) FATALE_ERREUR("fd=NULL processPereEsUnTinyShell",125);

    memset(procPereNom, 0, NOMBRE);
    read(fd,procPereNom, NOMBRE);
    
    printf("Pére du shell courrant [%s]\n", procPereNom);
    if ( !strcmp(procPereNom, "./tinyshell") ) resultat = 1;

    close(fd);
    free(procPereChemin);
    return resultat;

}