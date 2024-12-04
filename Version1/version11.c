/**
 * @file snake_game.c
 * @brief Jeu du serpent en C
 * @author [Votre Nom]
 * @version 1.0
 * @date 04/12/2024
 *
 * Implémentation du jeu du serpent respectant les conventions de codage.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <time.h>

// Taille du serpent
#define TAILLE_SERPENT 10

// Dimensions du plateau
#define LARGEUR_PLATEAU 80
#define HAUTEUR_PLATEAU 40

// Position initiale de la tête du serpent
#define POSITION_X_INIT 40
#define POSITION_Y_INIT 20

// Nombre de pommes pour gagner
#define NOMBRE_POMMES 10

// Temporisation entre deux déplacements (microsecondes)
#define TEMPORISATION 200000

// Caractères du jeu
#define CHAR_CORPS 'X'
#define CHAR_TETE 'O'
#define CHAR_BORDURE '#'
#define CHAR_VIDE ' '
#define CHAR_POMME '6'

// Touches de contrôle
#define TOUCHE_HAUT 'z'
#define TOUCHE_BAS 's'
#define TOUCHE_GAUCHE 'q'
#define TOUCHE_DROITE 'd'
#define TOUCHE_STOP 'a'

/**
 * @brief Plateau de jeu
 */
typedef char Plateau[LARGEUR_PLATEAU + 1][HAUTEUR_PLATEAU + 1];

// Variables globales
static int positionsPommesX[NOMBRE_POMMES] = {75, 75, 78, 2, 8, 78, 74, 2, 72, 5};
static int positionsPommesY[NOMBRE_POMMES] = {8, 39, 2, 2, 5, 39, 33, 38, 35, 2};
static int pommesMangees = 0;
static int deplacementsUnitaires = 0;

// Prototypes des fonctions
void initialiserPlateau(Plateau plateau);
void dessinerPlateau(const Plateau plateau);
void ajouterPomme(Plateau plateau);
void afficherCaractere(int x, int y, char caractere);
void effacerCaractere(int x, int y);
void dessinerSerpent(const int positionsX[], const int positionsY[]);
void deplacerSerpent(int positionsX[], int positionsY[], char direction, Plateau plateau, bool *collision, bool *pommeMangee);
void positionnerCurseur(int x, int y);
int verifierTouche();
void desactiverEcho();
void activerEcho();

/**
 * @brief Fonction principale
 * @return EXIT_SUCCESS en cas de succès
 */
int main() {
    time_t debut = time(NULL);
    
    int positionsX[TAILLE_SERPENT];
    int positionsY[TAILLE_SERPENT];
    char touche, direction;
    bool collision = false;
    bool victoire = false;
    bool pommeMangee = false;
    Plateau plateau;

    // Initialisation des positions du serpent
    for (int i = 0; i < TAILLE_SERPENT; i++) {
        positionsX[i] = POSITION_X_INIT - i;
        positionsY[i] = POSITION_Y_INIT;
    }

    // Initialisation du plateau
    initialiserPlateau(plateau);
    system("clear");
    dessinerPlateau(plateau);
    srand(time(NULL));
    ajouterPomme(plateau);

    dessinerSerpent(positionsX, positionsY);
    desactiverEcho();
    direction = TOUCHE_DROITE;
    touche = TOUCHE_DROITE;

    // Boucle de jeu
    do {
        if (touche == TOUCHE_GAUCHE || touche == TOUCHE_HAUT || touche == TOUCHE_BAS || touche == TOUCHE_DROITE) {
            direction = touche;
        }

        deplacerSerpent(positionsX, positionsY, direction, plateau, &collision, &pommeMangee);

        if (pommeMangee) {
            pommesMangees++;
            victoire = (pommesMangees == NOMBRE_POMMES);

            if (!victoire) {
                ajouterPomme(plateau);
            }
        }

        if (!victoire && !collision) {
            usleep(TEMPORISATION);
            if (verifierTouche()) {
                touche = getchar();
            }
        }
    } while (touche != TOUCHE_STOP && !collision && !victoire);

    activerEcho();
    positionnerCurseur(1, HAUTEUR_PLATEAU + 1);

    if (victoire) {
        time_t fin = time(NULL);
        printf("Victoire ! Votre serpent s'est déplacé %d fois.\n", deplacementsUnitaires);
        printf("Temps écoulé : %.0f secondes.\n", difftime(fin, debut));
    }

    return EXIT_SUCCESS;
}