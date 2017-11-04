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

// s est un char*
#define CARACTERE_SIGNE_OPERATION(s) ( (*s) == '+' || (*s) == '-' || (*s) == '*' || (*s) == '/')
#define CARACTERE_SEPARATON_ENTRE_COMMANDE(s) ((*s)=='\0' || (*s) == '\n' || (*s) == ' ')
#define CARACTERE_BOOLEAN_OU(s) ((*s) == '|' && *(s+1) == '|' && *(s+2) == '\0')
#define CARACTERE_BOOLEAN_ET(s) ((*s) == '&' && *(s+1) == '&' && *(s+2) == '\0')
#define CARACTERE_BOOLEAN(s) (CARACTERE_BOOLEAN_ET(s) || CARACTERE_BOOLEAN_OU(s))
#define CARACTERE_FINCOMMANDE(s) (*(s) == ';' && *(s+1) == '\0')
#define CARACTERE_TUBE(s) (*(s) == '|' && *(s+1) == '\0')
#define CARACTERE_VARIABLE(s) (*(s) == '$')
#define CARACTERE_SEPARATEUR_DE_COMMANDE(s) (CARACTERE_TUBE(s) || CARACTERE_FINCOMMANDE(s) || CARACTERE_BOOLEAN(s) )

#define CARACTERE_REDIRECTION_O_STDOUT(s) ( (*(s)) == '>' && (*(s+1)) == '\0')
#define CARACTERE_REDIRECTION_A_STDOUT(s) ( (*s) == '>' && *(s+1) == '>' && *(s+2) == '\0')
#define CARACTERE_REDIRECTION_O_STDERR(s) ( (*s) == '2' && *(s+1) == '>' && *(s+2) == '\0')
#define CARACTERE_REDIRECTION_A_STDERR(s) ( (*s) == '2' && *(s+1) == '>' && *(s+2) == '>' && *(s+3) == '\0')
#define CARACTERE_REDIRECTION_O_STDOUTERR(s) ( (*s) == '>' && *(s+1) == '&' && *(s+2) == '\0')
#define CARACTERE_REDIRECTION_A_STDOUTERR(s) ( (*s) == '>' && *(s+1) == '>' && *(s+2) == '&' && *(s+3) == '\0')
#define CARACTERE_REDIRECTION_STDIN(s) ( (*s) == '<' && *(s+1) == '\0')
#define CARACTERE_REDIRECTION(s) ( CARACTERE_REDIRECTION_O_STDOUT(s) || CARACTERE_REDIRECTION_A_STDOUT(s) ||\
CARACTERE_REDIRECTION_O_STDERR(s) || CARACTERE_REDIRECTION_A_STDERR(s) || CARACTERE_REDIRECTION_O_STDOUTERR(s) ||\
CARACTERE_REDIRECTION_A_STDOUTERR(s) || CARACTERE_REDIRECTION_STDIN(s))

#define CARACTERE_SEPARATEUR(s) ( CARACTERE_REDIRECTION(s) || CARACTERE_FINCOMMANDE(s) || CARACTERE_BOOLEAN(s))

#endif