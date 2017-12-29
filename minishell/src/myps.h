#ifndef DEF_MYPS_H
#define DEF_MYPS_H

#define ROUGE "\033[1;31m"
#define VERT "\033[1;32m"
#define JAUNE "\033[1;33m"
#define BLUE "\033[1;34m"
#define MAGENTA "\033[1;35m"
#define CYAN "\033[1;36m"
#define RESET "\033[0m"

void afficheEnCouleurProcesus(char*);
double calculUtilisationCPU(pid_t);
char* recuperationEtat(pid_t);
unsigned long long recuperationVsize(pid_t);
unsigned long recuperationRss(pid_t);
char* recuperationNomProcessus(pid_t);
char* recuperationTty(pid_t);
char* recuperationCommand(pid_t);
char* recuperationUserName(pid_t);
void listerPid();

#endif
