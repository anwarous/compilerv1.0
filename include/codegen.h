#ifndef CODEGEN_H
#define CODEGEN_H

#include <stdio.h>

#include "ast.h"
#include "symbol_table.h"

int codegen_program(FILE *out, Program *program, SymbolTable *symbols);

#endif
