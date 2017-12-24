#include "quivontbien.h"
#include "myps.h"

// cette fonction est dans utilitiesString.c
char* fusionner2(const char *mot1, const char *mot2){

    char *mot = (char*) malloc(sizeof(char)*(strlen(mot1)+strlen(mot2)+1));

    strcpy(mot, mot1);
    strcat(mot, mot2);
    strcat(mot, "\0");
    return mot;
}

/*
  Idée d'amélioration:
  Au lieu de faire cas par cas, on vérifie si le nom de l'état contient la lettre
  par exemple: R+ contient R, SNs contient S
  ne pas faire au cas par cas
*/
void afficheEnCouleurProcesus(int pid, char* etat){
  assert(etat);

  // Les variables comme R+ sont englobés dans l'état R, de même pour les autres états(Ss etc)
  if (!strcmp(etat, "R")){
    printf(ROUGE("USER %s PID %d STAT %s\n"), "Adam", pid, etat);
  }
  else if (!strcmp(etat, "R+")){
      printf(ROUGE("USER %s PID %d STAT %s\n"), "Adam", pid, etat);
    }
  else if (!strcmp(etat, "Z")){
    printf(VERT("USER %s PID %d STAT %s\n"), "Adam", pid, etat);
  }
  else if (!strcmp(etat, "T")){
    printf(JAUNE("USER %s PID %d STAT %s\n"), "Adam", pid, etat);
  }
  else if (!strcmp(etat, "S")){
    printf(CYAN("USER %s PID %d STAT %s\n"), "Adam", pid, etat);
  }
  else if (!strcmp(etat, "Ss")){
    printf(CYAN("USER %s PID %d STAT %s\n"), "Adam", pid, etat);
  }
  else if (!strcmp(etat, "SNs")){
    printf(CYAN("USER %s PID %d STAT %s\n"), "Adam", pid, etat);
  }
  else if (!strcmp(etat, "U")){
    printf(MAGENTA("USER %s PID %d STAT %s\n"), "Adam", pid, etat);
  }
  else if (!strcmp(etat, "Us")){
    printf(MAGENTA("USER %s PID %d STAT %s\n"), "Adam", pid, etat);
  }

}

/*
  ls /proc
	ps aux
	USER PID %CPU %MEM VSZ RSS TTY STAT START TIME COMMAND
  fonction en cours de réalisation
*/

double calculUtilisationCPU(pid_t pid){

  char* buffer;
  char* chaineTmp;
  char* ptrBuffer;
  char* ptrChaineTmp;
  char* pidToString;

  int fd;
  int cptEspace;
  long utime = 0;
  long stime = 0;
  long cutime = 0;
  long cstime = 0;
  long starttime = 0;

  // attendant un pid en mode osef
  pid_t pidd = getpid();

  double tempsUpTimeCPU;

  // calcul du temps d'utilisation du temps cpu
  buffer = (char*) malloc(TAILLEMAXCHAINE*sizeof(char));
  fd = open("/proc/uptime", O_RDONLY);
  if (fd == ERR) perror("parcourirProc - open"), exit(1);
  read(fd, buffer, TAILLEMAXCHAINE-1);

  chaineTmp = (char*) malloc(TAILLEMAXCHAINE*sizeof(char));
  ptrChaineTmp = chaineTmp;

  ptrBuffer = buffer;
  for(; *ptrBuffer != ' ';){
    *ptrChaineTmp++ = *ptrBuffer++;
  }

  tempsUpTimeCPU = atof(chaineTmp);

  free(chaineTmp);
  free(buffer);

  pidToString = (char*) malloc(NUMBEROFTHEBEAST*sizeof(char));
  chaineTmp = (char*) malloc(TAILLEMAXCHAINE*sizeof(char));
  sprintf(pidToString, "%d", pidd);
  chaineTmp = fusionner2("/proc/", pidToString);
  chaineTmp = fusionner2(chaineTmp, "/stat");

  fd = open(chaineTmp, O_RDONLY);
  if (fd == ERR) perror("calculUtilisationCPU - open"), exit(2);
  buffer = (char*) malloc(NUMBEROFTHEBEAST*sizeof(char));
  read(fd, buffer, NUMBEROFTHEBEAST-1);

  free(chaineTmp);
  free(pidToString);
  chaineTmp = (char*) malloc(TAILLEMAXCHAINE*sizeof(char));

  /*
   récupérations de:
    utime;
    stime;
    cutime;
    cstime;
    starttime;
    problème: les valeurs du processus donnent 0 -> calcul impossible ! comment faire ?
  */
  for(cptEspace = 0, ptrChaineTmp = chaineTmp, ptrBuffer = buffer; *ptrBuffer;){
    if (*ptrBuffer == ' ') ++cptEspace, ++ptrBuffer;

    // utime
    if (cptEspace == 11){

      for(; *ptrBuffer != ' '; ){
        *ptrChaineTmp++ = *ptrBuffer++;
      }

      utime = atoi(chaineTmp);
      free(chaineTmp);
      chaineTmp = (char*) malloc(TAILLEMAXCHAINE*sizeof(char));
      ptrChaineTmp = chaineTmp;
      ptrBuffer += 2;
      ++cptEspace;
    }

    // stime
    else if (cptEspace == 12){
      for(; *ptrBuffer != ' '; ){
        *ptrChaineTmp++ = *ptrBuffer++;
      }

      stime = atoi(chaineTmp);
      free(chaineTmp);
      chaineTmp = (char*) malloc(TAILLEMAXCHAINE*sizeof(char));
      ptrChaineTmp = chaineTmp;
      ++ptrBuffer;
      ptrBuffer += 2;
      ++cptEspace;
    }
    // cutime
    else if (cptEspace == 13){
      for(; *ptrBuffer != ' '; ){
        *ptrChaineTmp++ = *ptrBuffer++;
      }

      cutime = atoi(chaineTmp);
      free(chaineTmp);
      chaineTmp = (char*) malloc(TAILLEMAXCHAINE*sizeof(char));
      ptrChaineTmp = chaineTmp;
      ++ptrBuffer;
      ptrBuffer += 2;
      ++cptEspace;
    }

    // cstime
    else if (cptEspace == 14){
      for(; *ptrBuffer != ' '; ){
        *ptrChaineTmp++ = *ptrBuffer++;
      }

      cstime = atoi(chaineTmp);
      free(chaineTmp);
      chaineTmp = (char*) malloc(TAILLEMAXCHAINE*sizeof(char));
      ptrChaineTmp = chaineTmp;
      ++ptrBuffer;
      ptrBuffer += 2;
      ++cptEspace;
    }

    // starttime
    else if (cptEspace == 17){
      for(; *ptrBuffer != ' '; ){
        *ptrChaineTmp++ = *ptrBuffer++;
      }

      starttime = atoi(chaineTmp);
      free(chaineTmp);
      chaineTmp = (char*) malloc(TAILLEMAXCHAINE*sizeof(char));
      ptrChaineTmp = chaineTmp;
      ++ptrBuffer;
      break;
    }
    else ++ptrBuffer;

  }

  printf("utime: %ld\n", utime);
  printf("stime: %ld\n", stime);
  printf("cutime: %ld\n", cutime);
  printf("cstime: %ld\n", cstime);
  printf("starttime: %ld\n", starttime);

  free(buffer);
  free(chaineTmp);
  close(fd);
  return tempsUpTimeCPU;

}

unsigned long recuperationRss(){

	pid_t pid = getpid();
  char* pidToString = (char*) malloc(NUMBEROFTHEBEAST*sizeof(char));
  sprintf(pidToString, "%d", pid);
  char* chemin;
  chemin = fusionner2("/proc/", pidToString);
  DIR* directory = NULL;
  struct dirent* entry;

  int cptEspace = 0;

	char*	buffer;
  char* ptrBuffer;
	char* strRss = (char*) malloc(TAILLEMAXCHAINE*sizeof(char));
	char* ptrStrRss = strRss;

	int fd;
	unsigned long rss = -1;

	char* nomRepertoire;
	char* fichierProcALire;

  directory = opendir(chemin);
  if (directory != NULL){
    free(pidToString);

    /*
			On récupère l'état du processus dans /proc/[pidDuProcessus]/stat
		*/
    for(; (entry = readdir(directory)) != NULL; ){
			nomRepertoire = (char*) malloc(TAILLEMAXCHAINE*sizeof(char));
      strcpy(nomRepertoire, entry->d_name);

			fichierProcALire = (char*) malloc(TAILLEMAXCHAINE*sizeof(char));
			fichierProcALire = fusionner2(chemin, "/");
			fichierProcALire = fusionner2(fichierProcALire, nomRepertoire);

			// On ouvre juste les fichiers dont on a besoin
			if (!strcmp(nomRepertoire, "stat")){
				fd = open(fichierProcALire, O_RDONLY);
				if (fd == ERR) perror("recuperationEtat - open - stat"), exit(1);
				else{
					buffer = (char*) malloc(NUMBEROFTHEBEAST*sizeof(char));
					read(fd, buffer, NUMBEROFTHEBEAST-1);
          for(cptEspace = 0, ptrBuffer = buffer; cptEspace != 18; ++ptrBuffer){
            if (*ptrBuffer == ' ') ++cptEspace;
          }

					for(; *ptrBuffer != ' '; *ptrStrRss++ = *ptrBuffer++);
					*ptrStrRss = '\0';
					free(buffer);

					rss = strtoul(strRss, '\0', 10);
					return rss;

				}
			}

			free(fichierProcALire);
			free(nomRepertoire);
    }

    closedir(directory);
		return rss;
  }
  else perror("parcourirProc - opendir"), exit(3);

}

unsigned long long recuperationVSize(){

  pid_t pid = getpid();
  char* pidToString = (char*) malloc(NUMBEROFTHEBEAST*sizeof(char));
  sprintf(pidToString, "%d", pid);
  char* chemin;
  chemin = fusionner2("/proc/", pidToString);
  DIR* directory = NULL;
  struct dirent* entry;

  int cptEspace = 0;

	char*	buffer;
  char* ptrBuffer;
	char* strVsize = (char*) malloc(TAILLEMAXCHAINE*sizeof(char));
	char* ptrStrVsize = strVsize;

	int fd;
	unsigned long long vsize = -1;

	char* nomRepertoire;
	char* fichierProcALire;

  directory = opendir(chemin);
  if (directory != NULL){
    free(pidToString);

    /*
			On récupère l'état du processus dans /proc/[pidDuProcessus]/stat
		*/
    for(; (entry = readdir(directory)) != NULL; ){
			nomRepertoire = (char*) malloc(TAILLEMAXCHAINE*sizeof(char));
      strcpy(nomRepertoire, entry->d_name);

			fichierProcALire = (char*) malloc(TAILLEMAXCHAINE*sizeof(char));
			fichierProcALire = fusionner2(chemin, "/");
			fichierProcALire = fusionner2(fichierProcALire, nomRepertoire);

			// On ouvre juste les fichiers dont on a besoin
			if (!strcmp(nomRepertoire, "stat")){
				fd = open(fichierProcALire, O_RDONLY);

				if (fd == ERR) perror("recuperationEtat - open - stat"), exit(1);
				else{
					buffer = (char*) malloc(NUMBEROFTHEBEAST*sizeof(char));
					read(fd, buffer, NUMBEROFTHEBEAST-1);
          for(cptEspace = 0, ptrBuffer = buffer; cptEspace != 17; ++ptrBuffer){
            if (*ptrBuffer == ' ') ++cptEspace;
          }

					for(; *ptrBuffer != ' '; *ptrStrVsize++ = *ptrBuffer++);
					*ptrStrVsize = '\0';
					free(buffer);

					vsize = strtoull(strVsize, '\0', 10);
					return vsize;

				}
			}

			free(fichierProcALire);
			free(nomRepertoire);
    }

    closedir(directory);
		return vsize;
  }
  else perror("parcourirProc - opendir"), exit(3);

}

char* recuperationEtat(){

  pid_t pid = getpid();
  char* pidToString = (char*) malloc(NUMBEROFTHEBEAST*sizeof(char));
  sprintf(pidToString, "%d", pid);
  char* chemin;
  chemin = fusionner2("/proc/", pidToString);
  DIR* directory = NULL;
  struct dirent* entry;

  int cptEspace;

	char*	buffer;
  char* ptrBuffer;
	char* etat = (char*) malloc(TAILLEMAXCHAINE*sizeof(char));;
	char* ptrEtat = etat;

	int fd;

	char* nomRepertoire;
	char* fichierProcALire;

  directory = opendir(chemin);
  if (directory != NULL){
    free(pidToString);

    /*
			On récupère l'état du processus dans /proc/[pidDuProcessus]/stat
		*/
    for(; (entry = readdir(directory)) != NULL; ){
			nomRepertoire = (char*) malloc(TAILLEMAXCHAINE*sizeof(char));
      strcpy(nomRepertoire, entry->d_name);

			fichierProcALire = (char*) malloc(TAILLEMAXCHAINE*sizeof(char));
			fichierProcALire = fusionner2(chemin, "/");
			fichierProcALire = fusionner2(fichierProcALire, nomRepertoire);

			// On ouvre juste les fichiers dont on a besoin
			if (!strcmp(nomRepertoire, "stat")){
				fd = open(fichierProcALire, O_RDONLY);

				if (fd == ERR) perror("recuperationEtat - open - stat"), exit(1);
				else{
					buffer = (char*) malloc(TAILLEMAXCHAINE*sizeof(char));
					read(fd, buffer, TAILLEMAXCHAINE-1);

          for(cptEspace = 0, ptrBuffer = buffer; cptEspace != 2; ++ptrBuffer){
            if (*ptrBuffer == ' ') ++cptEspace;
          }

					for(; *ptrBuffer != ' '; *ptrEtat++ = *ptrBuffer++);
					*ptrEtat = '\0';
					free(buffer);
				}
			}

			free(fichierProcALire);
			free(nomRepertoire);
    }

    closedir(directory);
		return etat;
  }
  else perror("parcourirProc - opendir"), exit(3);
}

char* recuperationNomProcessus(){

  pid_t pid = getpid();
  char* pidToString = (char*) malloc(TAILLEMAXCHAINE*sizeof(char));
  sprintf(pidToString, "%d", pid);
  char* chemin;
  chemin = fusionner2("/proc/", pidToString);
  DIR* directory = NULL;
  struct dirent* entry;

  char* buffer;
  char* ptrBuffer;
  char* strTmp;
  char* ptrStrTmp;

  int fd;

  directory = opendir(chemin);
  if (directory != NULL){
    chemin = fusionner2(chemin, "/cmdline");
    fd = open(chemin, O_RDONLY);
    if (fd == ERR) perror("recuperationNomProcessus - opendir - open"), exit(2);
    else{
      buffer = (char*) malloc(NUMBEROFTHEBEAST*sizeof(char));
      ptrBuffer = buffer;
      read(fd, buffer, NUMBEROFTHEBEAST-1);
      return buffer;
    }

    close(directory);
    close(fd);

  }
  else perror("recuperationNomProcessus - opendir"), exit(1);

}

char* recuperationTty(){

  pid_t pid;
  char* buffer;
  char* ptrBuffer;
  char* str;
  char* ptrStr;
  char* chemin;
  char* pidToString;

  DIR* directory;
  struct dirent* entry;

  int fd;

  /*
    à modifier pour la suite car on a besoin de tous les pid des différents processus
    et non d'un seul
  */
  pid = getpid();
  TESTFORKOK(pid);

  chemin = (char*) malloc(TAILLEMAXCHAINE*sizeof(char));
  pidToString = (char*) malloc(TAILLEMAXCHAINE*sizeof(char));
  sprintf(pidToString, "%d", pid);
  strcpy(chemin, "/proc/");
  strcat(chemin, pidToString);
  strcat(chemin, "/fd/");
  strcat(chemin, "0");

  //if ( (directory = opendir(chemin)) != NULL){
    if ((fd = open(chemin, O_RDONLY)) == ERR) perror("RecuperationTty - opendir - open"), exit(2);
    else{
      buffer = (char*) malloc(NUMBEROFTHEBEAST*sizeof(char));
      ptrBuffer = buffer;
      //read(fd, buffer, NUMBEROFTHEBEAST-1);
      return chemin;
    }
    close(directory);
    close(fd);
//  }
  //else perror("recuperationTty - opendir"), exit(1);

}

int main(int argc, char** argv){

  //printf("Etat: %s\n", recuperationEtat());
  //printf("Vsz: %llu\n", recuperationVSize());
	//printf("Rss: %lu\n", recuperationRss());
  //printf("nom du processus: %s\n", recuperationNomProcessus());
  printf("TTY: %s\n", recuperationTty());


  exit(0);
}
