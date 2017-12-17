#include <ctype.h>

#ifndef __UTILITIES_REGEX_H__
#define __UTILITIES_REGEX_H__ 1

#define REGEX_SLASH '/'
#define REGEX_ANTISLASH_ZERO '\0'
#define REGEX_CROCHET_OUVRANT '['
#define REGEX_CROCHET_FERMANT ']'
#define REGEX_TILD '^'
#define REGEX_INTERVAL '-'
#define REGEX_QUESTION_MARK '?'
#define REGEX_ETOILE_MARK '*'

#define REGEX_REALLOC_EXTENTION 16

#define REGEX_ERREUR_INCONNUE "regex inconnu (abandon)\n"
#define REGEX_ERREUR_INCOMPLETE "regex inconnu '[<caractere>-' (abandon)\n"
#define REGEX_ERREUR_PLAGE_ANBIGUE "plage de regex inconnue prés de '[-]' (abandons)\n"
#define REGEX_ERREUR_PLAGE_VIDE "regex inconnu '[]' (abandon)\n"

#define EST_UNE_CARACTERE_REGEX(c) (c=='?' || c=='*' || c=='[' || c==']' || c=='~' || c=='-')
#define EST_UN_ALPHADIGIT(c) (isalpha(c) || isdigit(c))

int comparateurEstUnCaractereRegex(char c);
int preformatExecuterRegex(char ***bufferCommandes, int *argument, int *tailleMax, char *expresion);

#endif