#include "../src/quivontbien.h"

#include "allocationChaine.h"

char* bufferDepuisFileDescriptor(int fd){

    int nombreDeCaractereLue = 0,
        nombreTotalDeCaractereLue = 0,
        tailleDuBuffer = ALLOCATION_CHAINE_TAILLE_BUFFER;

    char *buffer,
         *bufferDynamique;

    buffer = (char*) malloc(sizeof(char)*ALLOCATION_CHAINE_TAILLE_BUFFER);

    bufferDynamique = buffer;

    do {
        //nombreDeCaractereLue = read(fd, bufferDynamique, ALLOCATION_CHAINE_EXTEND_BUFFER);
        fgets(bufferDynamique, ALLOCATION_CHAINE_EXTEND_BUFFER, stdin);
        nombreDeCaractereLue = strlen(bufferDynamique);

        nombreTotalDeCaractereLue += nombreDeCaractereLue;

        if ( (tailleDuBuffer - ALLOCATION_CHAINE_EXTEND_BUFFER) < nombreTotalDeCaractereLue){
            buffer = (char*) realloc(buffer, tailleDuBuffer += 2*ALLOCATION_CHAINE_EXTEND_BUFFER);
        }

        bufferDynamique = buffer + nombreTotalDeCaractereLue;

    } while (nombreDeCaractereLue == ALLOCATION_CHAINE_EXTEND_BUFFER);

    if (nombreTotalDeCaractereLue == tailleDuBuffer){
        buffer = (char*) realloc(buffer, tailleDuBuffer + 1);
    } 

    memset(buffer+nombreDeCaractereLue, 0, tailleDuBuffer - nombreDeCaractereLue-1);

    //printf("fd;%d & lu [%s] (%d caractéres), espace total de buffer %d\n", fd, buffer, nombreDeCaractereLue+1, tailleDuBuffer);
    return buffer;

}

char* bufferDepuisLentrerStandard(){

    return bufferDepuisFileDescriptor(STDIN_FILENO);
}