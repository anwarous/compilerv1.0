%option noyywrap nodefault yylineno

%{
#include <stdlib.h>
#include <string.h>

#include "parser.tab.h"

static char *dup_string(const char *src) {
    size_t n = strlen(src) + 1;
    char *out = (char *)malloc(n);
    if (out) {
        memcpy(out, src, n);
    }
    return out;
}
%}

DIGIT      [0-9]
ID         [A-Za-z_][A-Za-z0-9_]*
WS         [ \t\r]+

%%
{WS}                           ;
\n                             ;

"Algorithme"                  return ALGORITHME;
"Début"|"Debut"             return DEBUT;
"Fin"                         return FIN;
"TantQue"                     return TANTQUE;
"Faire"                       return FAIRE;
"FinTantQue"                  return FINTANTQUE;
"Si"                          return SI;
"Alors"                       return ALORS;
"Sinon"                       return SINON;
"FinSi"                       return FINSI;
"Lire"                        return LIRE;
"Écrire"|"Ecrire"           return ECRIRE;
"Mod"                         return MOD_OP;
"Div"                         return DIV_OP;

"<-"                          return ASSIGN;
"!="                          return NEQ;
"<="                          return LTE;
">="                          return GTE;
"="                           return EQ;
"<"                           return LT;
">"                           return GT;

"("                           return '(';
")"                           return ')';
","                           return ',';
"+"                           return '+';
"-"                           return '-';
"*"                           return '*';
"/"                           return '/';

"\""([^\\\n]|(\\.))*"\"" {
    yylval.str = dup_string(yytext);
    return STRING;
}

{DIGIT}+"."{DIGIT}+           {
    yylval.fval = atof(yytext);
    return REAL;
}

{DIGIT}+                       {
    yylval.ival = atol(yytext);
    return INT;
}

{ID}                           {
    yylval.str = dup_string(yytext);
    return IDENT;
}

.                              {
    return yytext[0];
}
%%
