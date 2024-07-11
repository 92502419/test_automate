#include "afnd.h"
#include <string.h> // Inclusion de la bibliothèque string.h pour utiliser strcmp

// Fonction pour charger un AFND à partir d'un fichier
void afnd_load(FILE* f, afnd* A) {
    int n, i, j;
    char buf[256];
    fscanf(f, "%d", &n); // Lire le nombre d'états
    A->nbs = n; // Stocker le nombre d'états dans la structure AFND
    A->fin = 0; // Initialiser les états finaux à zéro

    // Initialiser la matrice de transitions à zéro
    for (i = 0; i < n; ++i) {
        for (j = 0; j < 256; ++j) {
            A->tran[i][j] = 0;
        }
    }

    // Lire les transitions et les états finaux depuis le fichier
    while (fscanf(f, "%s", buf) == 1) {
        if (strcmp(buf, "final") == 0) { // Si la ligne indique un état final
            int state;
            fscanf(f, "%d", &state); // Lire l'état final
            A->fin |= (1 << state); // Mettre à jour le bit correspondant à cet état
        } else if (strcmp(buf, "trans") == 0) { // Si la ligne indique une transition
            int src, dst;
            char sym;
            fscanf(f, "%d %c %d", &src, &sym, &dst); // Lire la source, le symbole et la destination
            A->tran[src][(int)sym] |= (1 << dst); // Mettre à jour la matrice de transitions
        }
    }
}

// Fonction pour générer une représentation DOT de l'AFND
void afnd_dot(FILE* f, afnd* A) {
    int i, j;
    fprintf(f, "digraph {\n");
    fprintf(f, "rankdir=LR;\n");
    fprintf(f, "node [shape=doublecircle];");

    // Afficher les états finaux
    for (i = 0; i < A->nbs; ++i) {
        if (A->fin & (1 << i)) {
            fprintf(f, " %d", i);
        }
    }
    fprintf(f, ";\n");
    fprintf(f, "node [shape=circle];\n");

    // Afficher les transitions
    for (i = 0; i < A->nbs; ++i) {
        for (j = 0; j < 256; ++j) {
            if (A->tran[i][j]) {
                for (int k = 0; k < A->nbs; ++k) {
                    if (A->tran[i][j] & (1 << k)) {
                        fprintf(f, "%d -> %d [label=\"%c\"];\n", i, k, j);
                    }
                }
            }
        }
    }
    fprintf(f, "}\n");
}

// Fonction pour calculer les fermetures epsilon
void afnd_eps(afnd* A) {
    int i, j, k;
    for (i = 0; i < A->nbs; ++i) {
        for (j = 0; j < A->nbs; ++j) {
            if (A->tran[i]['e'] & (1 << j)) { // Si une transition epsilon existe
                for (k = 0; k < A->nbs; ++k) {
                    if (A->tran[j]['e'] & (1 << k)) { // Propager la fermeture epsilon
                        A->tran[i]['e'] |= (1 << k);
                    }
                }
            }
        }
    }
}

// Fonction pour vérifier si un ensemble est nouveau
int isnew(afnd* A, set s) {
    for (int i = 0; i < A->nbs; ++i) {
        if (A->tran[i][0] == s) {
            return 0; // L'ensemble n'est pas nouveau
        }
    }
    return 1; // L'ensemble est nouveau
}

// Fonction pour convertir un AFND en AFD
void afnd_det(afnd* A) {
    afnd B;
    int i, j, k, n;
    set tmp[MAX], newset;

    B.nbs = 0; // Initialiser le nombre d'états de l'AFD
    B.fin = 0; // Initialiser les états finaux de l'AFD
    for (i = 0; i < MAX; ++i) {
        for (j = 0; j < 256; ++j) {
            B.tran[i][j] = 0; // Initialiser la matrice de transitions de l'AFD
        }
    }

    tmp[0] = 1; // Initialiser le premier ensemble d'états
    B.tran[0][0] = tmp[0]; // Ajouter l'ensemble à l'AFD
    B.nbs = 1; // Mettre à jour le nombre d'états de l'AFD

    // Construire l'AFD à partir de l'AFND
    for (i = 0, n = 1; i < n; ++i) {
        for (j = 0; j < 256; ++j) {
            newset = 0;
            for (k = 0; k < A->nbs; ++k) {
                if (tmp[i] & (1 << k)) {
                    newset |= A->tran[k][j]; // Ajouter les états de transition
                }
            }
            if (newset && isnew(&B, newset)) {
                tmp[n] = newset; // Ajouter un nouvel ensemble d'états
                B.tran[n++][0] = newset; // Mettre à jour l'AFD
            }
            B.tran[i][j] = newset; // Mettre à jour les transitions
        }
    }

    // Mettre à jour les états finaux de l'AFD
    for (i = 0; i < n; ++i) {
        if (tmp[i] & A->fin) {
            B.fin |= (1 << i);
        }
    }

    // Copier les informations de l'AFD dans l'AFND
    A->nbs = B.nbs;
    A->fin = B.fin;
    for (i = 0; i < MAX; ++i) {
        for (j = 0; j < 256; ++j) {
            A->tran[i][j] = B.tran[i][j];
        }
    }
}

// Fonction pour minimiser un AFD
void afnd_min(afnd* A) {
    int i, j, k, n, p, b[MAX];
    set P[MAX], Q[MAX], W[MAX], X;

    // Initialiser les ensembles de partition
    for (i = 0, n = 0; i < A->nbs; ++i) {
        if (A->fin & (1 << i)) {
            P[n++] = 1 << i; // Ajouter les états finaux à P
        }
    }

    Q[0] = ~A->fin & ((1 << A->nbs) - 1); // Ajouter les états non finaux à Q
    W[0] = Q[0]; // Initialiser W

    // Construire les ensembles de partition
    while (n > 0) {
        X = W[--n];
        for (i = 0; i < 256; ++i) {
            p = 0;
            for (j = 0; j < A->nbs; ++j) {
                if (X & (1 << j)) {
                    for (k = 0; k < A->nbs; ++k) {
                        if (A->tran[j][i] & (1 << k)) {
                            b[p++] = k; // Ajouter les états de transition
                        }
                    }
                }
            }
            if (p > 0) {
                set Y = 0;
                for (j = 0; j < p; ++j) {
                    Y |= (1 << b[j]); // Créer un nouvel ensemble d'états
                }
                P[n++] = Y; // Ajouter Y à P
                W[n - 1] = Y; // Ajouter Y à W
            }
        }
    }

    // Réduire les états de l'AFD
    A->nbs = 0;
    for (i = 0; i < n; ++i) {
        for (j = 0; j < A->nbs; ++j) {
            if (P[i] == P[j]) {
                break;
            }
        }
        if (j == A->nbs) {
            P[A->nbs++] = P[i];
        }
    }

    // Mettre à jour les états finaux
    for (i = 0; i < A->nbs; ++i) {
        if (A->fin & P[i]) {
            A->fin |= (1 << i);
        } else {
            A->fin &= ~(1 << i);
        }
    }

        // Mettre à jour les transitions de l'AFD
    for (i = 0; i < A->nbs; ++i) {
        for (j = 0; j < 256; ++j) {
            A->tran[i][j] = 0; // Réinitialiser les transitions
            for (k = 0; k < A->nbs; ++k) {
                if (P[i] & (1 << k)) { // Si l'état k est dans l'ensemble P[i]
                    A->tran[i][j] |= A->tran[k][j]; // Copier la transition de l'état k
                }
            }
        }
    }
}

