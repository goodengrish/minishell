#ifndef __ALLOCATIONCHAINE_H__
#define __ALLOCATIONCHAINE_H__ 1

#define ALLOCATION_CHAINE_TAILLE_BUFFER 1024
#define ALLOCATION_CHAINE_EXTEND_BUFFER 512

char* bufferDepuisFileDescriptor(int fd);
char* bufferDepuisLentrerStandard();

#endif