CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -Iinclude
LEX = flex
YACC = bison

SRC = src/main.c src/ast.c src/symbol_table.c src/semantic.c src/codegen.c src/parser.tab.c src/lex.yy.c
OBJ = $(SRC:.c=.o)

all: algoc

src/parser.tab.c src/parser.tab.h: src/parser.y
	$(YACC) -d -o src/parser.tab.c src/parser.y

src/lex.yy.c: src/lexer.l src/parser.tab.h
	$(LEX) -o src/lex.yy.c src/lexer.l

algoc: src/parser.tab.c src/lex.yy.c $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ) -lfl

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f src/*.o src/parser.tab.c src/parser.tab.h src/lex.yy.c algoc generated/*.out

.PHONY: all clean
