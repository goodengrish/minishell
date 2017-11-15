#include "../mytinyshell.h"

#ifndef __PARSEURDECOMMANDE_H__
#define __PARSEURDECOMMANDE_H__ 1

#ifndef __MYTINISHELL_H__
#error "minishell.h doit être inclus dans parseurDeCommande.h"
#endif

#define PDC_ERREUR_SYMBOLE_INNATENDU "erreur de syntaxe prés du symbole innatendu %c (abandon)\n"
#define PDC_ERREUR_ANTISLASH_VIDE "Erreur symbole <\"> manquant (abandon)\n"
#define PDC_ERREUR_RESREGEX_NULL "Impossible d'acceder à '%s': Aucun fichier au dossier de ce genre\n"

#define PDC_ERREUR_EXECUTER_ERREUR_SYMBOLE_INNATENDU(car, buffer, bufferCommandes) {\
fprintf(stderr, "erreur de syntaxe prés du symbole innatendu %c (file:%s,line:%d) (abandon)\n", car, __FILE__, __LINE__);\
*buffer = RETOURALALIGNE;\
return bufferCommandes;}

#define PDC_ERREUR_EXECUTE_ANTISLASH_VIDE(buffer, bufferCommandes){\
fprintf(stderr, "Erreur symbole <\"> manquant (file:%s,line:%d) (abandon)\n", __FILE__, __LINE__);\
*buffer = RETOURALALIGNE;\
return bufferCommandes;}

#define PDC_ERREUR_EXECUTE_RESREGEX_NULL(car, buffer, bufferCommandes) {\
fprintf(stderr, "Impossible d'acceder à '%s': Aucun fichier au dossier de ce genre (file:%s,line:%d) (abandon)\n", car, __FILE__, __LINE__);\
*buffer = RETOURALALIGNE;\
return bufferCommandes;}

#endif