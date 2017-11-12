#include <ctype.h>

#ifndef __UTILITIES_REGEX_H__
#define __UTILITIES_REGEX_H__ 1

#define UN_CARACTERE(c) (c == '?')
#define PLUSIEUR_CARACTETE(c) (c == '*')
#define CROCHET_OUVRANT(c) (c == '[')
#define CROCHET_FERMANT(c) (c == ']')
#define IGNORE_NOT(c) (c == '~')
#define INTERVAL(c) (c == '-')

// s = char*
#define EST_UNE_CARACTERE_REGEX(s) (UN_CARACTERE(*s) || PLUSIEUR_CARACTETE(*s) || CROCHET_OUVRANT(*s) ||\
CROCHET_FERMANT(*s) || IGNORE_NOT(*s) || INTERVAL(*s))
#define EST_UN_ALPHADIGIT(c) (isalpha(c) || isdigit(c))

int comparateurEstUnCaractereRegex(char c);
char *retorunePrefexDeChaine(char *argv1, char **postFix);
int regexAvecEnsemble(char *chaine, char *regex, int not);
int regexValidePour(char *chaine, char *regex);
void executerRegex(char *repertoire, char *regex);

#endif