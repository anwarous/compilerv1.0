#include "symbol_table.h"

#include <stdlib.h>
#include <string.h>

static char *dup_string(const char *src) {
    size_t n = strlen(src) + 1;
    char *out = (char *)malloc(n);
    if (out) {
        memcpy(out, src, n);
    }
    return out;
}

void symbol_table_init(SymbolTable *table) { table->head = NULL; }

Symbol *symbol_table_lookup(SymbolTable *table, const char *name) {
    for (Symbol *sym = table->head; sym; sym = sym->next) {
        if (strcmp(sym->name, name) == 0) {
            return sym;
        }
    }
    return NULL;
}

Symbol *symbol_table_define(SymbolTable *table, const char *name, DataType type) {
    Symbol *existing = symbol_table_lookup(table, name);
    if (existing) {
        if (existing->type == TYPE_UNKNOWN) {
            existing->type = type;
        }
        return existing;
    }

    Symbol *sym = (Symbol *)calloc(1, sizeof(Symbol));
    sym->name = dup_string(name);
    sym->type = type;
    sym->next = table->head;
    table->head = sym;
    return sym;
}

void symbol_table_free(SymbolTable *table) {
    Symbol *sym = table->head;
    while (sym) {
        Symbol *next = sym->next;
        free(sym->name);
        free(sym);
        sym = next;
    }
    table->head = NULL;
}
