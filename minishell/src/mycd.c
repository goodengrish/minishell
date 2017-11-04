#include "quivontbien.h"
#include "mycd.h"

DIR* ouvrirRepertoire(const char* nomRepertoire){

  pid_t pid;

  int verifChdir = 1;

  pid = fork();
  TESTFORKOK(pid);

  if (!pid){
    verifChdir = chdir(nomRepertoire);

    if (!verifChdir){
      printf("dans le fils:\n");
      system("ls");
      exit(0);
    }
    else{
      perror("chdir failed"), exit(1);
    }

  }
  else{
    wait(NULL);
    printf("Dans le père:\n");
    system("ls");
  }

}

int main(int argc, char** argv){

  DIR* osef = ouvrirRepertoire("osef");

  exit(0);
}
