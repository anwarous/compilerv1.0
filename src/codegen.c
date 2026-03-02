#include "codegen.h"

#include <stdio.h>

static void emit_indent(FILE *out, int indent) {
    for (int i = 0; i < indent; i++) {
        fputs("    ", out);
    }
}

static const char *ctype(DataType type) {
    switch (type) {
        case TYPE_REAL:
            return "double";
        case TYPE_STRING:
            return "char*";
        case TYPE_INT:
        case TYPE_BOOL:
        case TYPE_UNKNOWN:
        case TYPE_ERROR:
            return "int";
    }
    return "int";
}

static const char *printf_fmt(DataType type) {
    switch (type) {
        case TYPE_REAL:
            return "%g";
        case TYPE_STRING:
            return "%s";
        default:
            return "%d";
    }
}

static const char *scanf_fmt(DataType type) {
    switch (type) {
        case TYPE_REAL:
            return "%lf";
        case TYPE_STRING:
            return "%s";
        default:
            return "%d";
    }
}

static void emit_expr(FILE *out, Expr *expr) {
    switch (expr->kind) {
        case EXPR_INT:
            fprintf(out, "%ld", expr->as.int_value);
            break;
        case EXPR_REAL:
            fprintf(out, "%g", expr->as.real_value);
            break;
        case EXPR_STRING:
            fprintf(out, "%s", expr->as.string_value);
            break;
        case EXPR_VAR:
            fprintf(out, "%s", expr->as.var_name);
            break;
        case EXPR_UNARY:
            fputs("(-", out);
            emit_expr(out, expr->as.unary.operand);
            fputc(')', out);
            break;
        case EXPR_BINARY:
            fputc('(', out);
            emit_expr(out, expr->as.binary.left);
            switch (expr->as.binary.op) {
                case OP_ADD: fputs(" + ", out); break;
                case OP_SUB: fputs(" - ", out); break;
                case OP_MUL: fputs(" * ", out); break;
                case OP_DIV: fputs(" / ", out); break;
                case OP_MOD: fputs(" % ", out); break;
                case OP_EQ: fputs(" == ", out); break;
                case OP_NEQ: fputs(" != ", out); break;
                case OP_LT: fputs(" < ", out); break;
                case OP_GT: fputs(" > ", out); break;
                case OP_LTE: fputs(" <= ", out); break;
                case OP_GTE: fputs(" >= ", out); break;
                case OP_NEG: break;
            }
            emit_expr(out, expr->as.binary.right);
            fputc(')', out);
            break;
    }
}

static void emit_stmt_list(FILE *out, Stmt *stmt, int indent, SymbolTable *symbols) {
    for (; stmt; stmt = stmt->next) {
        switch (stmt->kind) {
            case STMT_ASSIGN:
                emit_indent(out, indent);
                fprintf(out, "%s = ", stmt->as.assign.name);
                emit_expr(out, stmt->as.assign.value);
                fputs(";\n", out);
                break;
            case STMT_READ: {
                Symbol *sym = symbol_table_lookup(symbols, stmt->as.read.name);
                DataType type = sym ? sym->type : TYPE_INT;
                emit_indent(out, indent);
                fprintf(out, "scanf(\"%s\", &%s);\n", scanf_fmt(type), stmt->as.read.name);
                break;
            }
            case STMT_WRITE:
                for (ExprList *arg = stmt->as.write.args; arg; arg = arg->next) {
                    emit_indent(out, indent);
                    fprintf(out, "printf(\"%s\", ", printf_fmt(arg->expr->inferred_type));
                    emit_expr(out, arg->expr);
                    fputs(");\n", out);
                }
                emit_indent(out, indent);
                fputs("printf(\"\\n\");\n", out);
                break;
            case STMT_IF:
                emit_indent(out, indent);
                fputs("if ", out);
                emit_expr(out, stmt->as.if_stmt.condition);
                fputs(" {\n", out);
                emit_stmt_list(out, stmt->as.if_stmt.then_branch, indent + 1, symbols);
                emit_indent(out, indent);
                fputs("}", out);
                if (stmt->as.if_stmt.else_branch) {
                    fputs(" else {\n", out);
                    emit_stmt_list(out, stmt->as.if_stmt.else_branch, indent + 1, symbols);
                    emit_indent(out, indent);
                    fputs("}", out);
                }
                fputc('\n', out);
                break;
            case STMT_WHILE:
                emit_indent(out, indent);
                fputs("while ", out);
                emit_expr(out, stmt->as.while_stmt.condition);
                fputs(" {\n", out);
                emit_stmt_list(out, stmt->as.while_stmt.body, indent + 1, symbols);
                emit_indent(out, indent);
                fputs("}\n", out);
                break;
        }
    }
}

int codegen_program(FILE *out, Program *program, SymbolTable *symbols) {
    fputs("#include <stdio.h>\n\n", out);
    fputs("int main(void) {\n", out);

    for (Symbol *sym = symbols->head; sym; sym = sym->next) {
        fprintf(out, "    %s %s = 0;\n", ctype(sym->type), sym->name);
    }
    if (symbols->head) {
        fputc('\n', out);
    }

    emit_stmt_list(out, program->statements, 1, symbols);
    fputs("    return 0;\n", out);
    fputs("}\n", out);
    return 0;
}
