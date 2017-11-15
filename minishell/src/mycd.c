#include "quivontbien.h"
#include "mycd.h"

/*
  On vérifiera le nombre d'arguments donnés lors de l'appel de la commande cd
  si aucun répertoire n'est donné, on appelle la fonction sePlacerDansHome()
*/
void ouvrirRepertoire(const char* nomRepertoire){

  pid_t pid;

  int verifChdir = -(NUMBEROFTHEBEAST);
  int status;

  pid = fork();
  TESTFORKOK(pid);

  if (!pid){
    verifChdir = chdir(nomRepertoire);

    if (!verifChdir){

      // essai de commande dans le fils
      // fonctionne !
      printf("dans le fils:\n");
      system("ls");
      exit(1);
    }
    else{
      perror("chdir failed; directory unfound"), exit(2);
    }

  }
  else{
    wait(&status);

    if (WIFEXITED(status)){
      printf("Le processus fils s'est terminé normalement: %d.\n", WEXITSTATUS(status));
    }
    else{
      printf("Le processus fils s'est terminé anormalement.\n");
    }
    exit(0);
  }

}

void sePlacerDansHome(){

  pid_t pid;
  int status;
  int verifChdir = -(NUMBEROFTHEBEAST);

  pid = fork();
  TESTFORKOK(pid);

  if (!pid){
    
    // modifier le chemin d'accès
    verifChdir = chdir("home/");

    if (!verifChdir){
      printf("Processus fils: \n");
      system("ls");
      exit(0);
    }

    else{
      perror("redirection to home/ failed"), exit(1);
    }

  }
  else{
    wait(&status);

    if (WIFEXITED(status)){
      printf("Le processus s'est terminé normalement: %d\n", WEXITSTATUS(status));
    }
    else{
      printf("Le processus fils s'est terminé anormalement.\n");
    }

    exit(0);
  }

}

int executerCD(char *chemin){
	
	return !chdir(chemin);
}

int main(int argc, char** argv){

  // vérification du nb d'args pour les appels des fonctions

  //ouvrirRepertoire("osef");
  //sePlacerDansHome();

  exit(0);
}
