#include <stdio.h>
#include "afnd.h"

int main() {
    afnd A; // Déclaration d'une variable de type afnd
    FILE *inputFile = fopen("automate.txt", "r"); // Ouverture du fichier contenant la description de l'AFND en mode lecture
    FILE *dotFile; // Déclaration d'un pointeur de fichier pour le fichier de sortie DOT

    if (inputFile == NULL) {
        // Vérification de l'ouverture du fichier, si l'ouverture échoue, afficher une erreur et retourner 1
        perror("Erreur lors de l'ouverture du fichier");
        return 1;
    }

    afnd_load(inputFile, &A); // Chargement de l'AFND à partir du fichier
    fclose(inputFile); // Fermeture du fichier d'entrée

    dotFile = fopen("automate.dot", "w"); // Ouverture du fichier DOT en mode écriture
    if (dotFile == NULL) {
        // Vérification de l'ouverture du fichier DOT, si l'ouverture échoue, afficher une erreur et retourner 1
        perror("Erreur lors de l'ouverture du fichier dot");
        return 1;
    }

    afnd_dot(dotFile, &A); // Génération de la représentation DOT de l'AFND
    fclose(dotFile); // Fermeture du fichier DOT

    afnd_eps(&A); // Calcul des fermetures epsilon de l'AFND
    afnd_det(&A); // Conversion de l'AFND en AFD
    afnd_min(&A); // Minimisation de l'AFD

    dotFile = fopen("automate_min.dot", "w"); // Ouverture du fichier DOT pour l'AFD minimisé en mode écriture
    if (dotFile == NULL) {
        // Vérification de l'ouverture du fichier DOT, si l'ouverture échoue, afficher une erreur et retourner 1
        perror("Erreur lors de l'ouverture du fichier dot");
        return 1;
    }

    afnd_dot(dotFile, &A); // Génération de la représentation DOT de l'AFD minimisé
    fclose(dotFile); // Fermeture du fichier DOT

    printf("Automate traité et sauvegardé dans automate_min.dot\n"); // Affichage d'un message indiquant que l'automate a été traité et sauvegardé

    return 0; // Retourne 0 pour indiquer que le programme s'est terminé correctement
}
