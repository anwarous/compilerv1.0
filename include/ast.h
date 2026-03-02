#ifndef AST_H
#define AST_H

#include <stddef.h>

typedef enum {
    TYPE_UNKNOWN,
    TYPE_INT,
    TYPE_REAL,
    TYPE_STRING,
    TYPE_BOOL,
    TYPE_ERROR
} DataType;

typedef enum {
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,
    OP_MOD,
    OP_EQ,
    OP_NEQ,
    OP_LT,
    OP_GT,
    OP_LTE,
    OP_GTE,
    OP_NEG
} Operator;

typedef enum {
    EXPR_INT,
    EXPR_REAL,
    EXPR_STRING,
    EXPR_VAR,
    EXPR_BINARY,
    EXPR_UNARY
} ExprKind;

typedef struct Expr Expr;
typedef struct ExprList ExprList;
typedef struct Stmt Stmt;

typedef enum {
    STMT_ASSIGN,
    STMT_READ,
    STMT_WRITE,
    STMT_IF,
    STMT_WHILE
} StmtKind;

struct Expr {
    ExprKind kind;
    int line;
    DataType inferred_type;
    union {
        long int_value;
        double real_value;
        char *string_value;
        char *var_name;
        struct {
            Operator op;
            Expr *left;
            Expr *right;
        } binary;
        struct {
            Operator op;
            Expr *operand;
        } unary;
    } as;
};

struct ExprList {
    Expr *expr;
    ExprList *next;
};

struct Stmt {
    StmtKind kind;
    int line;
    Stmt *next;
    union {
        struct {
            char *name;
            Expr *value;
        } assign;
        struct {
            char *name;
        } read;
        struct {
            ExprList *args;
        } write;
        struct {
            Expr *condition;
            Stmt *then_branch;
            Stmt *else_branch;
        } if_stmt;
        struct {
            Expr *condition;
            Stmt *body;
        } while_stmt;
    } as;
};

typedef struct {
    char *name;
    Stmt *statements;
} Program;

Program *ast_make_program(char *name, Stmt *statements);
Stmt *ast_append_stmt(Stmt *list, Stmt *stmt);
ExprList *ast_append_expr(ExprList *list, Expr *expr);

Stmt *ast_make_assign(char *name, Expr *value, int line);
Stmt *ast_make_read(char *name, int line);
Stmt *ast_make_write(ExprList *args, int line);
Stmt *ast_make_if(Expr *condition, Stmt *then_branch, Stmt *else_branch, int line);
Stmt *ast_make_while(Expr *condition, Stmt *body, int line);

Expr *ast_make_int(long value, int line);
Expr *ast_make_real(double value, int line);
Expr *ast_make_string(char *value, int line);
Expr *ast_make_var(char *name, int line);
Expr *ast_make_binary(Operator op, Expr *left, Expr *right, int line);
Expr *ast_make_unary(Operator op, Expr *operand, int line);

void ast_free_program(Program *program);

#endif
