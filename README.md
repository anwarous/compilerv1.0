# Compilateur de pseudo-code algorithmique (FR) vers C

Ce projet implémente un compilateur complet en **C + Flex + Bison** qui traduit un pseudo-code de style français en code C compilable.

## Architecture

- `src/lexer.l` : analyse lexicale (mots-clés, identifiants, constantes, opérateurs).
- `src/parser.y` : grammaire Bison, construction de l'AST.
- `src/ast.c`, `include/ast.h` : structures AST et gestion mémoire.
- `src/semantic.c` : analyse sémantique (types, variables non déclarées, opérations illégales).
- `src/codegen.c` : génération de C (`while`, `if/else`, `scanf`, `printf`, opérateurs).
- `src/main.c` : pipeline compilation (parse → sémantique → génération).
- `examples/*.alg` : exemples d'entrée.
- `generated/*.c` : sorties C générées par le compilateur.

## Grammaire (BNF simplifiée)

```bnf
<program>      ::= "Algorithme" <ident> "Début" <stmt_list> "Fin"
<stmt_list>    ::= <stmt_list> <stmt> | ε
<stmt>         ::= <ident> "<-" <expr>
                 | "Lire" "(" <ident> ")"
                 | "Écrire" "(" <write_args> ")"
                 | "TantQue" "(" <condition> ")" "Faire" <stmt_list> "FinTantQue"
                 | "Si" "(" <condition> ")" "Alors" <stmt_list> <optional_else> "FinSi"
<optional_else>::= "Sinon" <stmt_list> | ε
<write_args>   ::= <write_args> "," <expr> | <expr>
<condition>    ::= <expr> <relop> <expr> | <expr>
<relop>        ::= "=" | "!=" | "<" | ">" | "<=" | ">="
<expr>         ::= <expr> "+" <expr>
                 | <expr> "-" <expr>
                 | <expr> "*" <expr>
                 | <expr> "/" <expr>
                 | <expr> "Div" <expr>
                 | <expr> "Mod" <expr>
                 | "-" <expr>
                 | "(" <expr> ")"
                 | <ident> | <int> | <real> | <string>
```

## Règles de traduction

- `TantQue (...) Faire ... FinTantQue` → `while (...) { ... }`
- `Si (...) Alors ... Sinon ... FinSi` → `if (...) { ... } else { ... }`
- `Mod` → `%`
- `Div` → `/`
- `Lire(x)` → `scanf("%d", &x)` (ou `%lf` selon type inféré)
- `Écrire(...)` → `printf(...)`

## Analyse sémantique

- Inférence de type automatique: `int`, `double`, `string`.
- Détection de variable non déclarée (utilisée avant affectation/lecture).
- Vérification des opérations:
  - `Mod` exige des entiers.
  - Opérations arithmétiques exigent des numériques.
  - Comparaisons entre types compatibles.

Les erreurs sont affichées avec numéro de ligne.

## Compilation du compilateur

Prérequis: `flex`, `bison`, `gcc`.

```bash
make
```

Binaire produit: `./algoc`

## Utilisation

```bash
./algoc examples/inverse.alg generated/inverse.c
./algoc examples/parite_signe.alg generated/parite_signe.c
```

Puis compiler le C généré:

```bash
gcc -Wall -Wextra -std=c11 generated/inverse.c -o generated/inverse.out
gcc -Wall -Wextra -std=c11 generated/parite_signe.c -o generated/parite_signe.out
```
