#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "ast.h"
#include "symbol_table.h"

typedef struct {
    int error_count;
} SemanticResult;

SemanticResult semantic_check_program(Program *program, SymbolTable *symbols);

#endif
