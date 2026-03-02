#include <stdio.h>
#include <stdlib.h>

#include "ast.h"
#include "codegen.h"
#include "semantic.h"
#include "symbol_table.h"

extern FILE *yyin;
extern int yyparse(void);
extern Program *g_program;

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <input.alg> <output.c>\n", argv[0]);
        return 1;
    }

    yyin = fopen(argv[1], "r");
    if (!yyin) {
        perror("Cannot open input file");
        return 1;
    }

    if (yyparse() != 0 || !g_program) {
        fclose(yyin);
        return 1;
    }
    fclose(yyin);

    SymbolTable symbols;
    symbol_table_init(&symbols);

    SemanticResult semantic = semantic_check_program(g_program, &symbols);
    if (semantic.error_count > 0) {
        ast_free_program(g_program);
        symbol_table_free(&symbols);
        return 1;
    }

    FILE *out = fopen(argv[2], "w");
    if (!out) {
        perror("Cannot open output file");
        ast_free_program(g_program);
        symbol_table_free(&symbols);
        return 1;
    }

    codegen_program(out, g_program, &symbols);
    fclose(out);

    ast_free_program(g_program);
    symbol_table_free(&symbols);
    return 0;
}
