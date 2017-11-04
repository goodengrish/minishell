#include <errno.h>
#include <stdlib.h>

#ifndef __CONST_MYTINISHELL_H__
#define __CONST_MYTINISHELL_H__ 1

#define REPERTOIR_BIN_PERSO "./bin"
#define REPERTOIR_BIN_SH    "/bin/sh"
#define ENVP_REPERTOIR_HOME "HOME"

#define ENVP_REPERTOIR_COURRANT_PERSO "MYCD"
#define ENVP_STATUS_PERSO "MYSTATUS"
#define ENVP_STATUS_PID_PERSO "MYSTATUSPID"

#define CODE_OU_LOGIQUE '|'
#define CODE_ET_LOGIQUE '&'
#define CODE_TUBE       'T'
#define CODE_FIN_LIGNE  ';'
#define CODE_DOLLAR     '$'

#define FATALSYSERROR(x,y) perror(x), exit(y)

#define CARACTERE_SIGNE_OPERATION(s) ( (*s) == '+' || (*s) == '-' || (*s) == '*' || (*s) == '/')
#define CARACTERE_SEPARATON_ENTRE_COMMANDE(s) ((*s)=='\0' || (*s) == '\n' || (*s) == ' ')
#define CARACTERE_BOOLEAN_OU(s) ((*s) == CODE_OU_LOGIQUE && (*(s+1) == CODE_OU_LOGIQUE))
#define CARACTERE_BOOLEAN_ET(s) ((*s) == CODE_ET_LOGIQUE && (*(s+1) == CODE_ET_LOGIQUE))
#define CARACTERE_BOOLEAN(s) (CARACTERE_BOOLEAN_ET(s) || CARACTERE_BOOLEAN_OU(s))
#define CARACTERE_FINCOMMANDE(s) (*(s) == CODE_FIN_LIGNE)
#define CARACTERE_TUBE(s) (*(s) == CODE_OU_LOGIQUE )
#define CARACTERE_VARIABLE(s) (*(s) == CODE_DOLLAR)
#define CARACTERE_SEPARATEUR_DE_COMMANDE(s) (CARACTERE_TUBE(s) || CARACTERE_FINCOMMANDE(s) || CARACTERE_BOOLEAN(s) )

#endif