---
title: |
    | Systèmes Informatiques - Rapport TP3
    | ultra-cp
author: Hugo Haldi
date: 3 novembre 2020
---

## Compilation

Pour compiler le code, un Makefile est disponible: 

```bash
make ultra-cp
```

## 1 ultra-ls

La première partie du TP concerne la fonctionnalité de listing des fichiers, nommé *ultra-ls*. L'en-tête des fonctions se trouve dans `include/ultra-ls.h` et le code source dans `src/ultra-ls.c`.

La fonction `ultra-ls` prend en paramètre un chemin d'accès, et si le chemin est un dossier, affiche le contenu du dossier, sinon le fichier. Les propriétés du fichier suivantes sont affichées:

`mode taille date chemin`

Exemple avec le dossier courant:

```text
drwxr-xr-x      4096 Sun Nov  1 14:41:20 2020   .
drwxr-xr-x      4096 Sun Nov  1 13:43:33 2020   ./include
-rw-r--r--    178223 Fri Oct 16 22:08:34 2020   ./TP3.pdf
-rw-r--r--      1067 Sun Nov  1 14:29:54 2020   ./LICENSE
-rw-r--r--         0 Sun Nov  1 14:41:20 2020   ./Makefile
-rw-r--r--        49 Sun Nov  1 14:29:54 2020   ./README.md
-rw-r--r--      1947 Fri Oct 16 22:15:31 2020   ./.clang-format
-rw-r--r--       208 Tue Oct 27 17:31:16 2020   ./CMakeLists.txt
drwxr-xr-x      4096 Sun Nov  1 14:22:26 2020   ./src
drwxr-xr-x      4096 Sun Nov  1 14:46:21 2020   ./cmake-build-debug
drwxr-xr-x      4096 Sun Nov  1 14:32:32 2020   ./.git
-rw-r--r--     65750 Sun Nov  1 14:38:30 2020   ./Report.pdf
-rw-r--r--       118 Sun Nov  1 14:38:28 2020   ./Report.md
drwxr-xr-x      4096 Sun Nov  1 14:46:39 2020   ./.idea
-rw-r--r--       222 Sun Nov  1 14:39:16 2020   ./.gitignore
```

Dans l'exécutable de `main`, le listing n'est effectué que si *ultra_cp* est appelé avec un seul argument. Sinon la procédure [*ultra-cp*](#2-ultra-cp) est exécutée.

### 1.1 Fonctionnement

*ultra-ls* teste d'abord si le chemin passé en paramètre est un dossier. Si c'est le cas, on liste tous les éléments du dossier avec la méthode `print_file`. Sinon on affiche le fichier directement avec `print_file`.

## 2 ultra-cp

*ultra-cp* permet de copier des fichiers/dossiers d'un répertoire de départ, cers un répertoire d'arrivée. Le fichier d'en-tête est dans `include/ultra-cp.h` et le code source dans `src/ultra-cp.c`.

La méthode `ultra-cp` doit être appelée avec une liste des fichiers sources, la taille de la liste, le chemin de destination, et des flags (0, *U_AFLAG* ou *U_FFLAG*).

Le programme fonctionne de la manière suivante:

- Si la destination est un fichier, mais que plusieurs sources sont données, on retourne une erreur
- Si la source et la destination existent et sont des dossiers, on copie le dossier source à l'intérieur du dossier de destination
- Si la source contient plusieurs fichiers ou que c'est un dossier, on crée le dossier de destionation si il n'existe pas
- Pour tous les fichiers sources, copier vers la destination:
  - si la source est un dossier, copier tous ses fichiers dans un nouveau répertoire dans la destination
  - si la source est plus récente que la destination, remplacer le fichier

### 2.1 Flags

Le programme implémente deux flags: `-f` et `-a`.

Le flag `-a` modifie les permissions du fichier de destination si il existe déjà et est identique au fichier source. Les fichiers de destination prendront donc les mêmes permissions que les fichiers sources, qu'ils existent déjà ou non

Le flag `-f` copie les liens en tant que tel, sans les suivre, en transformant le chemin du lien en chemin absolu si il est relatif.
