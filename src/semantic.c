#include "semantic.h"

#include <stdarg.h>
#include <stdio.h>

static void semantic_error(SemanticResult *result, int line, const char *fmt, ...) {
    va_list args;
    fprintf(stderr, "Semantic error at line %d: ", line);
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    fprintf(stderr, "\n");
    result->error_count++;
}

static int is_numeric(DataType type) { return type == TYPE_INT || type == TYPE_REAL; }

static DataType unify_numeric(DataType left, DataType right) {
    if (left == TYPE_REAL || right == TYPE_REAL) {
        return TYPE_REAL;
    }
    return TYPE_INT;
}

static DataType infer_expr(Expr *expr, SymbolTable *symbols, SemanticResult *result) {
    if (!expr) {
        return TYPE_ERROR;
    }

    switch (expr->kind) {
        case EXPR_INT:
        case EXPR_REAL:
        case EXPR_STRING:
            return expr->inferred_type;
        case EXPR_VAR: {
            Symbol *sym = symbol_table_lookup(symbols, expr->as.var_name);
            if (!sym) {
                semantic_error(result, expr->line, "undeclared variable '%s'", expr->as.var_name);
                expr->inferred_type = TYPE_ERROR;
            } else {
                expr->inferred_type = sym->type;
            }
            return expr->inferred_type;
        }
        case EXPR_UNARY: {
            DataType operand = infer_expr(expr->as.unary.operand, symbols, result);
            if (expr->as.unary.op == OP_NEG) {
                if (!is_numeric(operand)) {
                    semantic_error(result, expr->line, "unary '-' expects numeric operand");
                    expr->inferred_type = TYPE_ERROR;
                } else {
                    expr->inferred_type = operand;
                }
            }
            return expr->inferred_type;
        }
        case EXPR_BINARY: {
            DataType left = infer_expr(expr->as.binary.left, symbols, result);
            DataType right = infer_expr(expr->as.binary.right, symbols, result);
            Operator op = expr->as.binary.op;

            if (op == OP_ADD || op == OP_SUB || op == OP_MUL || op == OP_DIV) {
                if (!is_numeric(left) || !is_numeric(right)) {
                    semantic_error(result, expr->line, "arithmetic operators require numeric operands");
                    expr->inferred_type = TYPE_ERROR;
                } else {
                    expr->inferred_type = unify_numeric(left, right);
                }
            } else if (op == OP_MOD) {
                if (left != TYPE_INT || right != TYPE_INT) {
                    semantic_error(result, expr->line, "Mod requires integer operands");
                    expr->inferred_type = TYPE_ERROR;
                } else {
                    expr->inferred_type = TYPE_INT;
                }
            } else {
                if ((is_numeric(left) && is_numeric(right)) || left == right) {
                    expr->inferred_type = TYPE_BOOL;
                } else {
                    semantic_error(result, expr->line, "incompatible types in comparison");
                    expr->inferred_type = TYPE_ERROR;
                }
            }
            return expr->inferred_type;
        }
    }
    return TYPE_ERROR;
}

static void check_stmt_list(Stmt *stmt, SymbolTable *symbols, SemanticResult *result) {
    for (; stmt; stmt = stmt->next) {
        switch (stmt->kind) {
            case STMT_ASSIGN: {
                DataType rhs = infer_expr(stmt->as.assign.value, symbols, result);
                Symbol *sym = symbol_table_lookup(symbols, stmt->as.assign.name);
                if (!sym) {
                    symbol_table_define(symbols, stmt->as.assign.name, rhs);
                } else if (sym->type == TYPE_UNKNOWN) {
                    sym->type = rhs;
                } else if (rhs != TYPE_UNKNOWN && rhs != TYPE_ERROR && sym->type != rhs) {
                    if (!(is_numeric(sym->type) && is_numeric(rhs))) {
                        semantic_error(result, stmt->line, "type mismatch for variable '%s'", stmt->as.assign.name);
                    } else {
                        sym->type = unify_numeric(sym->type, rhs);
                    }
                }
                break;
            }
            case STMT_READ: {
                Symbol *sym = symbol_table_lookup(symbols, stmt->as.read.name);
                if (!sym) {
                    symbol_table_define(symbols, stmt->as.read.name, TYPE_INT);
                }
                break;
            }
            case STMT_WRITE: {
                for (ExprList *arg = stmt->as.write.args; arg; arg = arg->next) {
                    infer_expr(arg->expr, symbols, result);
                }
                break;
            }
            case STMT_IF: {
                DataType cond = infer_expr(stmt->as.if_stmt.condition, symbols, result);
                if (cond == TYPE_STRING) {
                    semantic_error(result, stmt->line, "if condition cannot be a string");
                }
                check_stmt_list(stmt->as.if_stmt.then_branch, symbols, result);
                check_stmt_list(stmt->as.if_stmt.else_branch, symbols, result);
                break;
            }
            case STMT_WHILE: {
                DataType cond = infer_expr(stmt->as.while_stmt.condition, symbols, result);
                if (cond == TYPE_STRING) {
                    semantic_error(result, stmt->line, "while condition cannot be a string");
                }
                check_stmt_list(stmt->as.while_stmt.body, symbols, result);
                break;
            }
        }
    }
}

SemanticResult semantic_check_program(Program *program, SymbolTable *symbols) {
    SemanticResult result = {0};
    check_stmt_list(program->statements, symbols, &result);
    return result;
}
