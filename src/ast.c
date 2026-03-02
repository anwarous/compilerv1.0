#include "ast.h"

#include <stdlib.h>
#include <string.h>

static void ast_free_stmt_list(Stmt *stmt);
static void ast_free_expr(Expr *expr);
static void ast_free_expr_list(ExprList *list);

Program *ast_make_program(char *name, Stmt *statements) {
    Program *program = (Program *)calloc(1, sizeof(Program));
    program->name = name;
    program->statements = statements;
    return program;
}

Stmt *ast_append_stmt(Stmt *list, Stmt *stmt) {
    if (!list) {
        return stmt;
    }
    Stmt *cur = list;
    while (cur->next) {
        cur = cur->next;
    }
    cur->next = stmt;
    return list;
}

ExprList *ast_append_expr(ExprList *list, Expr *expr) {
    ExprList *node = (ExprList *)calloc(1, sizeof(ExprList));
    node->expr = expr;
    if (!list) {
        return node;
    }
    ExprList *cur = list;
    while (cur->next) {
        cur = cur->next;
    }
    cur->next = node;
    return list;
}

Stmt *ast_make_assign(char *name, Expr *value, int line) {
    Stmt *stmt = (Stmt *)calloc(1, sizeof(Stmt));
    stmt->kind = STMT_ASSIGN;
    stmt->line = line;
    stmt->as.assign.name = name;
    stmt->as.assign.value = value;
    return stmt;
}

Stmt *ast_make_read(char *name, int line) {
    Stmt *stmt = (Stmt *)calloc(1, sizeof(Stmt));
    stmt->kind = STMT_READ;
    stmt->line = line;
    stmt->as.read.name = name;
    return stmt;
}

Stmt *ast_make_write(ExprList *args, int line) {
    Stmt *stmt = (Stmt *)calloc(1, sizeof(Stmt));
    stmt->kind = STMT_WRITE;
    stmt->line = line;
    stmt->as.write.args = args;
    return stmt;
}

Stmt *ast_make_if(Expr *condition, Stmt *then_branch, Stmt *else_branch, int line) {
    Stmt *stmt = (Stmt *)calloc(1, sizeof(Stmt));
    stmt->kind = STMT_IF;
    stmt->line = line;
    stmt->as.if_stmt.condition = condition;
    stmt->as.if_stmt.then_branch = then_branch;
    stmt->as.if_stmt.else_branch = else_branch;
    return stmt;
}

Stmt *ast_make_while(Expr *condition, Stmt *body, int line) {
    Stmt *stmt = (Stmt *)calloc(1, sizeof(Stmt));
    stmt->kind = STMT_WHILE;
    stmt->line = line;
    stmt->as.while_stmt.condition = condition;
    stmt->as.while_stmt.body = body;
    return stmt;
}

Expr *ast_make_int(long value, int line) {
    Expr *expr = (Expr *)calloc(1, sizeof(Expr));
    expr->kind = EXPR_INT;
    expr->line = line;
    expr->inferred_type = TYPE_INT;
    expr->as.int_value = value;
    return expr;
}

Expr *ast_make_real(double value, int line) {
    Expr *expr = (Expr *)calloc(1, sizeof(Expr));
    expr->kind = EXPR_REAL;
    expr->line = line;
    expr->inferred_type = TYPE_REAL;
    expr->as.real_value = value;
    return expr;
}

Expr *ast_make_string(char *value, int line) {
    Expr *expr = (Expr *)calloc(1, sizeof(Expr));
    expr->kind = EXPR_STRING;
    expr->line = line;
    expr->inferred_type = TYPE_STRING;
    expr->as.string_value = value;
    return expr;
}

Expr *ast_make_var(char *name, int line) {
    Expr *expr = (Expr *)calloc(1, sizeof(Expr));
    expr->kind = EXPR_VAR;
    expr->line = line;
    expr->inferred_type = TYPE_UNKNOWN;
    expr->as.var_name = name;
    return expr;
}

Expr *ast_make_binary(Operator op, Expr *left, Expr *right, int line) {
    Expr *expr = (Expr *)calloc(1, sizeof(Expr));
    expr->kind = EXPR_BINARY;
    expr->line = line;
    expr->inferred_type = TYPE_UNKNOWN;
    expr->as.binary.op = op;
    expr->as.binary.left = left;
    expr->as.binary.right = right;
    return expr;
}

Expr *ast_make_unary(Operator op, Expr *operand, int line) {
    Expr *expr = (Expr *)calloc(1, sizeof(Expr));
    expr->kind = EXPR_UNARY;
    expr->line = line;
    expr->inferred_type = TYPE_UNKNOWN;
    expr->as.unary.op = op;
    expr->as.unary.operand = operand;
    return expr;
}

static void ast_free_stmt_list(Stmt *stmt) {
    while (stmt) {
        Stmt *next = stmt->next;
        switch (stmt->kind) {
            case STMT_ASSIGN:
                free(stmt->as.assign.name);
                ast_free_expr(stmt->as.assign.value);
                break;
            case STMT_READ:
                free(stmt->as.read.name);
                break;
            case STMT_WRITE:
                ast_free_expr_list(stmt->as.write.args);
                break;
            case STMT_IF:
                ast_free_expr(stmt->as.if_stmt.condition);
                ast_free_stmt_list(stmt->as.if_stmt.then_branch);
                ast_free_stmt_list(stmt->as.if_stmt.else_branch);
                break;
            case STMT_WHILE:
                ast_free_expr(stmt->as.while_stmt.condition);
                ast_free_stmt_list(stmt->as.while_stmt.body);
                break;
        }
        free(stmt);
        stmt = next;
    }
}

static void ast_free_expr(Expr *expr) {
    if (!expr) {
        return;
    }
    switch (expr->kind) {
        case EXPR_STRING:
            free(expr->as.string_value);
            break;
        case EXPR_VAR:
            free(expr->as.var_name);
            break;
        case EXPR_BINARY:
            ast_free_expr(expr->as.binary.left);
            ast_free_expr(expr->as.binary.right);
            break;
        case EXPR_UNARY:
            ast_free_expr(expr->as.unary.operand);
            break;
        case EXPR_INT:
        case EXPR_REAL:
            break;
    }
    free(expr);
}

static void ast_free_expr_list(ExprList *list) {
    while (list) {
        ExprList *next = list->next;
        ast_free_expr(list->expr);
        free(list);
        list = next;
    }
}

void ast_free_program(Program *program) {
    if (!program) {
        return;
    }
    free(program->name);
    ast_free_stmt_list(program->statements);
    free(program);
}
