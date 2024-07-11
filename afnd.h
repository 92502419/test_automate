#ifndef AFND_H
#define AFND_H

#include <stdio.h>
#include <stdlib.h>

#define MAX 64  // Limite maximale des états, augmentée à 64

typedef unsigned long long set; // Définition d'un ensemble d'états comme un entier long non signé

// Structure représentant un automate fini non déterministe (AFND)
typedef struct {
    int nbs;    // Nombre d'états
    set fin;    // Ensemble des états acceptants
    set tran[MAX][256];  // Matrice de transitions, où chaque état peut avoir des transitions sur 256 symboles possibles
} afnd;

// Prototypes des fonctions pour manipuler un AFND
void afnd_load(FILE*, afnd*); // Charger un AFND à partir d'un fichier
void afnd_dot(FILE*, afnd*); // Générer une représentation DOT d'un AFND
void afnd_eps(afnd*); // Calculer les fermetures epsilon d'un AFND
void afnd_det(afnd*); // Convertir un AFND en AFD
void afnd_min(afnd*); // Minimiser un AFD

#endif // AFND_H
