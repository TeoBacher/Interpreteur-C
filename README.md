# Projet interpréteur en C

L'objectif de ce projet est de créer un ***interpréteur*** simple.

## Utilisation

Compilation :


```bash
gcc -o NOM_DE_LEXECUTABLE lexer.c parser.c input.c
```

Execution : ```./NOM_DE_LEXECUTABLE```

Vous devez choisir entre l'execution en mode intéractif ou en mode fichier.

### Mode debug

Dans le fichier `lexer.h`, vous pouvez définir la variable `DEBUG` à `1` pour activer le mode debug.

```c
#define DEBUG 1
```

Ce mode affiche chaque étape de l'analyse syntaxique (parser, lexer et création de l'arbre).

## Fonctionnalités

- [x] Arithmétique simple
- [x] Variables
- [x] Priorités des opérations
- [x] Conditions
- [x] Typage
- [x] Lecture de fichiers
- [x] Boucles
- [x] Commentaires
- [ ] Fonctions
- [ ] Chaînes de caractères
