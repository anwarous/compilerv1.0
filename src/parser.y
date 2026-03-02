%{
#include <stdio.h>
#include <stdlib.h>

#include "ast.h"

extern int yylex(void);
extern int yylineno;
void yyerror(const char *msg);

Program *g_program = NULL;
%}

%code requires {
#include "ast.h"
}

%union {
    char *str;
    long ival;
    double fval;
    Expr *expr;
    ExprList *expr_list;
    Stmt *stmt;
    Operator op;
}

%token <str> IDENT STRING
%token <ival> INT
%token <fval> REAL
%token ALGORITHME DEBUT FIN
%token TANTQUE FAIRE FINTANTQUE
%token SI ALORS SINON FINSI
%token LIRE ECRIRE
%token MOD_OP DIV_OP
%token ASSIGN
%token EQ NEQ LT GT LTE GTE

%left EQ NEQ LT GT LTE GTE
%left '+' '-'
%left '*' '/' MOD_OP DIV_OP
%right UMINUS

%type <stmt> stmt stmt_list optional_else
%type <expr> expr condition
%type <expr_list> write_args
%type <op> relop

%%

program
    : ALGORITHME IDENT DEBUT stmt_list FIN {
        g_program = ast_make_program($2, $4);
      }
    ;

stmt_list
    : stmt_list stmt { $$ = ast_append_stmt($1, $2); }
    | /* empty */ { $$ = NULL; }
    ;

stmt
    : IDENT ASSIGN expr { $$ = ast_make_assign($1, $3, yylineno); }
    | LIRE '(' IDENT ')' { $$ = ast_make_read($3, yylineno); }
    | ECRIRE '(' write_args ')' { $$ = ast_make_write($3, yylineno); }
    | TANTQUE '(' condition ')' FAIRE stmt_list FINTANTQUE {
        $$ = ast_make_while($3, $6, yylineno);
      }
    | SI '(' condition ')' ALORS stmt_list optional_else FINSI {
        $$ = ast_make_if($3, $6, $7, yylineno);
      }
    ;

optional_else
    : SINON stmt_list { $$ = $2; }
    | /* empty */ { $$ = NULL; }
    ;

write_args
    : write_args ',' expr { $$ = ast_append_expr($1, $3); }
    | expr { $$ = ast_append_expr(NULL, $1); }
    ;

condition
    : expr relop expr { $$ = ast_make_binary($2, $1, $3, yylineno); }
    | expr { $$ = $1; }
    ;

relop
    : EQ { $$ = OP_EQ; }
    | NEQ { $$ = OP_NEQ; }
    | LT { $$ = OP_LT; }
    | GT { $$ = OP_GT; }
    | LTE { $$ = OP_LTE; }
    | GTE { $$ = OP_GTE; }
    ;

expr
    : expr '+' expr { $$ = ast_make_binary(OP_ADD, $1, $3, yylineno); }
    | expr '-' expr { $$ = ast_make_binary(OP_SUB, $1, $3, yylineno); }
    | expr '*' expr { $$ = ast_make_binary(OP_MUL, $1, $3, yylineno); }
    | expr '/' expr { $$ = ast_make_binary(OP_DIV, $1, $3, yylineno); }
    | expr DIV_OP expr { $$ = ast_make_binary(OP_DIV, $1, $3, yylineno); }
    | expr MOD_OP expr { $$ = ast_make_binary(OP_MOD, $1, $3, yylineno); }
    | '-' expr %prec UMINUS { $$ = ast_make_unary(OP_NEG, $2, yylineno); }
    | '(' expr ')' { $$ = $2; }
    | IDENT { $$ = ast_make_var($1, yylineno); }
    | INT { $$ = ast_make_int($1, yylineno); }
    | REAL { $$ = ast_make_real($1, yylineno); }
    | STRING { $$ = ast_make_string($1, yylineno); }
    ;

%%

void yyerror(const char *msg) {
    fprintf(stderr, "Syntax error at line %d: %s\n", yylineno, msg);
}
