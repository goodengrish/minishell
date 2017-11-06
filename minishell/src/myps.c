#include "quivontbien.h"
#include "myps.h"

void afficheProcesus(int pid, char* etat){
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

int main(int argc, char** argv){

  int pidosef = getpid();
  printf("PID: %d\n", pidosef);

  exit(0);
}
