#ifndef DEF_MYPS_H
#define DEF_MYPS_H

#define ROUGE(m) "\033[01;31m"m"\033[0m"
#define VERT(m) "\033[22;32m"m"\033[0m"
#define JAUNE(m) "\033[01;33m"m"\033[0m"
#define BLUE(m) "\033[22;34m"m"\033[0m"
#define MAGENTA(m) "\033[22;35m"m"\033[0m"
#define CYAN(m) "\033[22;36m"m"\033[0m"
#define RESET(m) "\033[22;30m"m"\033[0m"

void afficheEnCouleurProcesus(int, char*);
double calculUtilisationCPU(pid_t);
char* recuperationEtat();
unsigned long long recuperationVsize();
unsigned long recuperationRss();
char* recuperationNomProcessus();
char* recuperationTty();

#endif
