#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include "ast.h"

typedef struct Symbol {
    char *name;
    DataType type;
    struct Symbol *next;
} Symbol;

typedef struct {
    Symbol *head;
} SymbolTable;

void symbol_table_init(SymbolTable *table);
void symbol_table_free(SymbolTable *table);
Symbol *symbol_table_lookup(SymbolTable *table, const char *name);
Symbol *symbol_table_define(SymbolTable *table, const char *name, DataType type);

#endif
