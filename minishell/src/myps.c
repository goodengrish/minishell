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

void afficheEnCouleurProcesus(char* etat){
  assert(etat);

  // Les variables comme R+ sont englobés dans l'état R, de même pour les autres états(Ss etc)
  if (!strcmp(etat, "R")){
	printf("\033[1;31m");
    printf("%s ", etat);
	printf("\033[0m");
  }

  else if (!strcmp(etat, "R+")){
	printf("\033[1;31m");
    printf("%s ", etat);
	printf("\033[0m");
    }
  else if (!strcmp(etat, "Z")){
	printf("\033[1;32m");
    printf("%s ", etat);
	printf("\033[0m");
  }
  else if (!strcmp(etat, "T")){
	printf("\033[1;33m");
    printf("%s", etat);
	printf("\033[0m");
  }
  else if (!strcmp(etat, "S")){
	printf("\033[1;36m");
    printf("%s ", etat);
	printf("\033[0m");
  }
  else if (!strcmp(etat, "Ss")){
	printf("\033[1;36m");
    printf("%s ", etat);
	printf("\033[0m");
  }
  else if (!strcmp(etat, "SNs")){
	printf("\033[1;36m");
    printf("%s ", etat);
	printf("\033[0m");
  }
  else if (!strcmp(etat, "U")){
	printf("\033[1;35m");
    printf("%s ", etat);
	printf("\033[0m");
  }
  else if (!strcmp(etat, "Us")){
	printf("\033[1;35m");
    printf("%s ", etat);
	printf("\033[0m");
  }

}

double recuperationCpuUsage(pid_t pid){

	char* pathPidStat = (char*) malloc(TAILLEMAXCHAINE*sizeof(char));
	char* pathStat = "/proc/stat";
	char* pidToString = (char*) malloc(TAILLEMAXCHAINE*sizeof(char));
	char* buffer1 = (char*) malloc(NUMBEROFTHEBEAST*sizeof(char));
	char* buffer2 = (char*) malloc(NUMBEROFTHEBEAST*sizeof(char));
	char* str;

	char* ptr1;
	char* ptr2;
	char* ptrStr;
	
	long unsigned utime_before = -1;
	long unsigned utime_after = -1;
	long unsigned stime_before = -1;
	long unsigned stime_after = -1;

	double user_util = 0.0;
	double sys_util = 0.0;

	long time_total_before = 0;
	long time_total_after = 0;

	int fd1;
	int fd2;

	int cptEspace;
	
	sprintf(pidToString, "%d", pid);
	strcpy(pathPidStat, "/proc/");
	strcat(pathPidStat, pidToString);
	strcat(pathPidStat, "/stat");
	
	fd1 = open(pathStat, O_RDONLY);
	fd2 = open(pathPidStat, O_RDONLY);
	if (fd1 != ERR && fd2 != ERR){

		if (read(fd1, buffer1, NUMBEROFTHEBEAST-1) == ERR) perror("recuperationCpuUsage - read"), exit(3);
		if (read(fd2, buffer2, NUMBEROFTHEBEAST-1) == ERR) perror("recuperationCpuUsage - read"), exit(4);
		
		ptr1 = buffer1;
		ptr2 = buffer2;

		// récupération des infos pour time_total dans /proc/stat à l'aide du file descriptor fd1
		for(cptEspace = 0; cptEspace != 12;){
			if (cptEspace == 0 && *ptr1 == ' '){
				++cptEspace;
			}
			else if (cptEspace == 1 && *ptr1 == ' '){
				++cptEspace;
			}
			else if (cptEspace == 2 && *ptr1 == ' '){
				++ptr1;
				++ptr1;

				while (cptEspace != 12){
					str = (char*) malloc(TAILLEMAXCHAINE*sizeof(char));
					ptrStr = str;

					while (*ptr1 != ' '){
						*ptrStr++ = *ptr1++;
					}

					time_total_before += strtol(str, &ptrStr, 10);
					free(str);
					++cptEspace;
					++ptr1;
				}				

				break;
			}
			else ++ptr1;
		}

		// récupération de utime et stime dans /proc/[pid]/stat à l'aide du file descriptor fd2
		for(cptEspace = 0, ptrStr = str ; cptEspace != 13;){
			if (*ptr2 == ' ' && cptEspace != 13){
				++cptEspace;
				++ptr2;
			}
			else if (*ptr2 != ' ' && cptEspace != 13){
				++ptr2;
			}
		}
		
		// on chope utime
		str = (char*) malloc(TAILLEMAXCHAINE*sizeof(char));
		ptrStr = str;
		while(*ptr2 != ' '){
			*ptrStr++ = *ptr2++;
		}
		
		++ptr2;
		utime_before = strtol(str, &ptrStr, 10);
		
		free(str);
		str = (char*) malloc(TAILLEMAXCHAINE*sizeof(char));
		ptrStr = str;
		
		while(*ptr2 != ' '){
			*ptrStr++ = *ptr2++;
		}

		stime_before = strtol(str, &ptrStr, 10);
		free(str);

		// on met en pose le processus via sleep() et on récupére à nouveau les valeurs
		sleep(1);

		
		free(buffer1);
		free(buffer2);
		close(fd1);
		close(fd2);

		buffer1 = (char*) malloc(NUMBEROFTHEBEAST*sizeof(char));
		buffer2 = (char*) malloc(NUMBEROFTHEBEAST*sizeof(char));
		
		fd1 = open(pathStat, O_RDONLY);
		fd2 = open(pathPidStat, O_RDONLY);
		
		if (read(fd1, buffer1, NUMBEROFTHEBEAST-1) == ERR) perror("recuperationCpuUsage - read"), exit(5);
		if (read(fd2, buffer2, NUMBEROFTHEBEAST-1) == ERR) perror("recuperationCpuUsage - read"), exit(6);

		ptr1 = buffer1;
		ptr2 = buffer2;
		
		if (fd1 != ERR && fd2 != ERR){
			for(cptEspace = 0; cptEspace != 12;){
				if (cptEspace == 0 && *ptr1 == ' '){
					++cptEspace;
				}
				else if (cptEspace == 1 && *ptr1 == ' '){
					++cptEspace;
				}
				else if (cptEspace == 2 && *ptr1 == ' '){
					++ptr1;
					++ptr1;

					while (cptEspace != 12){
						str = (char*) malloc(TAILLEMAXCHAINE*sizeof(char));
						ptrStr = str;

						while (*ptr1 != ' '){
							*ptrStr++ = *ptr1++;
						}

						time_total_after += strtol(str, &ptrStr, 10);
						free(str);
						++cptEspace;
						++ptr1;
					}				

					break;
				}
				else ++ptr1;
			}

			// récupération de utime et stime dans /proc/[pid]/stat à l'aide du file descriptor fd2
			for(cptEspace = 0, ptrStr = str ; cptEspace != 13;){
				if (*ptr2 == ' ' && cptEspace != 13){
					++cptEspace;
					++ptr2;
				}
				else if (*ptr2 != ' ' && cptEspace != 13){
					++ptr2;
				}
			}
		
			// on chope utime
			str = (char*) malloc(TAILLEMAXCHAINE*sizeof(char));
			ptrStr = str;
			while(*ptr2 != ' '){
				*ptrStr++ = *ptr2++;
			}
		
			++ptr2;
			utime_after = strtol(str, &ptrStr, 10);
		
			free(str);
			str = (char*) malloc(TAILLEMAXCHAINE*sizeof(char));
			ptrStr = str;
		
			while(*ptr2 != ' '){
				*ptrStr++ = *ptr2++;
			}

			stime_after = strtol(str, &ptrStr, 10);
			free(str);

			//printf("||||||||||||||||||||||||||||||\ntime_total_before: %lu | time_total_after: %lu | utime_before: %lu | utime_after: %lu | stime_before: %lu | stime_after: %lu\n",
				//time_total_before, time_total_after, utime_before, utime_after, stime_before, stime_after);
			
			user_util = 100 * (utime_after - utime_before) / (time_total_after - time_total_before);
			sys_util = 100 * (stime_after - stime_before) / (time_total_after - time_total_before);
			return user_util + sys_util;
			
		}
		else perror("recupeationCpuUsage - "), exit(2);
		
	}
	else perror("recuperationCpuUsage - open"), exit(1);
	
}

unsigned long recuperationRss(pid_t pid){

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
				if (read(fd, buffer, NUMBEROFTHEBEAST-1) == ERR) perror("recuperationRss - read"), exit(3);
          		for(cptEspace = 0, ptrBuffer = buffer; cptEspace != 23; ++ptrBuffer){
            		if (*ptrBuffer == ' ') ++cptEspace;
          		}

				for(; *ptrBuffer != ' '; *ptrStrRss++ = *ptrBuffer++);
					*ptrStrRss = '\0';
					free(buffer);

					rss = strtoul(strRss, '\0', 10);
					close(fd);
					free(pidToString);
					free(strRss);
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

unsigned long long recuperationVSize(pid_t pid){

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
					if (read(fd, buffer, NUMBEROFTHEBEAST-1) == ERR) perror("recuperationVSize - read"), exit(3);
          for(cptEspace = 0, ptrBuffer = buffer; cptEspace != 22; ++ptrBuffer){
            if (*ptrBuffer == ' ') ++cptEspace;
          }

					for(; *ptrBuffer != ' '; *ptrStrVsize++ = *ptrBuffer++);
					*ptrStrVsize = '\0';
					free(buffer);

					vsize = strtoull(strVsize, '\0', 10);
					close(fd);
					closedir(directory);
					free(pidToString);
					free(strVsize);
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

char* recuperationEtat(pid_t pid){

	char* buffer;
	char* chemin;
	char* pidToString;
	char* etat;
	char* ptrEtat;

	int fd;

	int i;
	
	chemin = (char*) malloc(TAILLEMAXCHAINE*sizeof(char));
	pidToString = (char*) malloc(TAILLEMAXCHAINE*sizeof(char));

	sprintf(pidToString, "%d", pid);
	strcpy(chemin, "/proc/");
	strcat(chemin, pidToString);
	strcat(chemin, "/status");
	
	fd = open(chemin, O_RDONLY);
	if (fd != ERR){
		buffer = (char*) malloc(NUMBEROFTHEBEAST*sizeof(char));
		etat = (char*) malloc(TAILLEMAXCHAINE*sizeof(char));
		
		if (read(fd, buffer, NUMBEROFTHEBEAST-1) == ERR) perror("recuperationUserName - open - read"), exit(1);

		for(i = 0; i < NUMBEROFTHEBEAST-1 ; ++i){
			if (buffer[i] == 'S' && buffer[i+1] == 't' && buffer[i+2] == 'a' && buffer[i+3] == 't' && buffer[i+4] == 'e' ) break;
		}

		i += 7;

		for(ptrEtat = etat; buffer[i] != '\t' && buffer[i] != ' ';){
			*ptrEtat++ = buffer[i++];
		}

		*ptrEtat = '\0';
		close(fd);

		return etat;
		
	}
	else perror("recuperationUserName - open"), exit(2);
}

char* recuperationCommand(pid_t pid){

	char* buffer;
	char* chemin;
	char* pidToString;

	char* command;

	int fd;

	int i;
	int j;
	
	chemin = (char*) malloc(TAILLEMAXCHAINE*sizeof(char));
	pidToString = (char*) malloc(TAILLEMAXCHAINE*sizeof(char));

	sprintf(pidToString, "%d", pid);
	strcpy(chemin, "/proc/");
	strcat(chemin, pidToString);
	strcat(chemin, "/status");
	
	fd = open(chemin, O_RDONLY);
	if (fd != ERR){
		buffer = (char*) malloc(NUMBEROFTHEBEAST*sizeof(char));
		command = (char*) malloc(NUMBEROFTHEBEAST*sizeof(char));

		if (read(fd, buffer, NUMBEROFTHEBEAST-1) == ERR) perror("recuperationCommand - read"), exit(3);

		for(i = 6, j = 0; buffer[i] != '\t' && buffer[i] != ' ' && buffer[i] != '\b' && buffer[i] != '\n'; ++i){
			command[j++] = buffer[i];
		}
		
		free(buffer);
		free(chemin);
		free(pidToString);
		close(fd);
		return command;

	}

	else perror("recuperationCommand - open"), exit(2);

}

// à terminer
char* recuperationStartTime(pid_t pid){
	char* buffer;
	char* path = (char*) malloc(TAILLEMAXCHAINE*sizeof(char));
	char* pidToString = (char*) malloc(TAILLEMAXCHAINE*sizeof(char));

	int fd;

	sprintf(pidToString, "%d", pid);
	strcpy(path, "/proc/");
	strcat(path, pidToString);
	strcat(path, "/stat");
	
	fd = open(path, O_RDONLY);
	if (fd != ERR){
		
		buffer = (char*) malloc(NUMBEROFTHEBEAST*sizeof(char));
		
		if (read(fd, buffer, NUMBEROFTHEBEAST-1) == ERR) perror("recuperationStartTime - read"), exit(3);
		return buffer;
	}
	else perror("recuperationStartTime - open"), exit(2);
	
}

char* recuperationUserName(pid_t pid){

	struct passwd* pwd;

	uid_t uid;
	char* buffer;	
	char* chemin;
	char* pidToString;
	char* uidString;
	char* ptrUid;

	int fd;

	int i;
	
	chemin = (char*) malloc(TAILLEMAXCHAINE*sizeof(char));
	pidToString = (char*) malloc(TAILLEMAXCHAINE*sizeof(char));

	sprintf(pidToString, "%d", pid);
	strcpy(chemin, "/proc/");
	strcat(chemin, pidToString);
	strcat(chemin, "/status");
	
	fd = open(chemin, O_RDONLY);
	if (fd != ERR){
		buffer = (char*) malloc(NUMBEROFTHEBEAST*sizeof(char));
		uidString = (char*) malloc(TAILLEMAXCHAINE*sizeof(char));
		
		if (read(fd, buffer, NUMBEROFTHEBEAST-1) == ERR) perror("recuperationUserName - open - read"), exit(1);

		for(i = 0; i < NUMBEROFTHEBEAST-1 ; ++i){
			if (buffer[i] == 'U' && buffer[i+1] == 'i' && buffer[i+2] == 'd') break;
		}

		i += 5;

		for(ptrUid = uidString; buffer[i] != '\t' && buffer[i] != ' ';){
			*ptrUid++ = buffer[i++];
		}
		
		
		uid = strtol(uidString, &ptrUid, 10);

		pwd = getpwuid(uid);

		close(fd);
		free(buffer);
		free(uidString);

		return pwd->pw_name;
		
	}
	else perror("recuperationUserName - open"), exit(2);
}

void listerPid(){
	int pid = 1;
	char* pidToString;
	char* str;
	char* command;
	char* buffer;
	char* chemin;
	int fd;
	int fd2;

	printf("USER\tPID\tSTAT\tVSZ\tRSS\tCOMMAND\n\n");
	for(; pid != 65534; ++pid){
		pidToString = (char*) malloc(TAILLEMAXCHAINE*sizeof(char));
		sprintf(pidToString, "%d", pid);
		str = (char*) malloc(TAILLEMAXCHAINE*sizeof(char));
		strcpy(str, "/proc/");
		strcat(str, pidToString);

		if (((fd = open(str, O_RDONLY)) != ERR)){

			chemin = (char*) malloc(TAILLEMAXCHAINE*sizeof(char));
			buffer = (char*) malloc(NUMBEROFTHEBEAST*sizeof(char));

			strcpy(chemin, "/proc/");
			strcat(chemin, pidToString);
			strcat(chemin, "/status");

			if (((fd2 = open(chemin, O_RDONLY)) == ERR)) perror("listerPid - open - open"), exit(1);

			if (read(fd2, buffer, NUMBEROFTHEBEAST-1) == ERR) perror("listerPid - open - read"), exit(2);

			close(fd);
			close(fd2);

			if (strstr(buffer, "State") != NULL){
				
				command = (char*) malloc(TAILLEMAXCHAINE*sizeof(char));

				printf("%s\t", recuperationUserName(pid));
				printf("%d\t", pid);
				command = recuperationEtat(pid);
				afficheEnCouleurProcesus(command);
			 	printf("%llu\t", recuperationVSize(pid));
				printf("%lu\t", recuperationRss(pid));
				printf("%s\t", recuperationCommand(pid));
				//printf("TTY: %s\n", recuperationTty(pid));
				//printf("Start_time: %s\n", recuperationStartTime(pid));
				//printf("CPU usage: %f\n", recuperationCpuUsage(pid));

				printf("\n\n");

				free(command);
			}
			else printf("NOPE\n");
			free(chemin);
			free(buffer);
		}
		
		free(pidToString);
	}

}

int main(int argc, char** argv){

	listerPid();

	exit(0);
}
