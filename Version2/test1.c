/**
 * @file version2.c
 * @brief Automatisation d'un jeu snake codé en C lors de la SAÉ 1.02
 * @author Chauvel Arthur, Le Chevère Yannis
 * @version 2.0
 * @date 16/12/2024
 *
 * Automatisation d'un jeu snake pour qu'il se déplace automatiquement.
 * Version 2 avec les modifications suivantes :
 * - Bordures percées en leur milieu permettant au serpent de passer d'un côté à l'autre
 * - Interdiction de faire demi-tour ou de se croiser
 */

/* Fichiers inclus */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <time.h>

// taille du serpent
#define TAILLE 10
// dimensions du plateau
#define LARGEUR_PLATEAU 80	
#define HAUTEUR_PLATEAU 40
// position initiale de la tête du serpent
#define X_INITIAL 40
#define Y_INITIAL 20
// nombre de pommes à manger pour gagner
#define NB_POMMES 10
// temporisation entre deux déplacements du serpent (en microsecondes)
#define ATTENTE 200000
// caractères pour représenter le serpent
#define CORPS 'X'
#define TETE 'O'
// touches de direction ou d'arrêt du jeu
#define HAUT 'z'
#define BAS 's'
#define GAUCHE 'q'
#define DROITE 'd'
#define STOP 'a'
// caractères pour les éléments du plateau
#define BORDURE '#'
#define VIDE ' '
#define POMME '6'
// nombre servant a la convertion du temps en seconde
#define CONVERTION_SECONDE 1000

// Nouvel indicateur pour interdire le demi-tour
#define AUCUNE_DIRECTION 0

int lesPommesX[NB_POMMES] = {75, 75, 78, 2, 8, 78, 74,  2, 72, 5};
int lesPommesY[NB_POMMES] = { 8, 39,  2, 2, 5, 39, 33, 38, 35, 2};
typedef char tPlateau[LARGEUR_PLATEAU+1][HAUTEUR_PLATEAU+1];

// compteur de pommes mangées
int nbPommes = 0;
int nbDepUnitaires = 0;

/* Déclaration des prototypes de fonctions */
void initPlateau(tPlateau plateau);
void dessinerPlateau(tPlateau plateau);
void ajouterPomme(tPlateau plateau);
void afficher(int, int, char);
void effacer(int x, int y);
void dessinerSerpent(int lesX[], int lesY[]);
void progresser(int lesX[], int lesY[], char* direction, tPlateau plateau, bool * collision, bool * pomme);
void gotoxy(int x, int y);
int kbhit();
void disable_echo();
void enable_echo();
char trouverNouvelleDirection(int lesX[], int lesY[], char ancienneDirection);
bool estDirectionInterdite(char nouvelleDirection, char ancienneDirection);

int main()
{
    // 2 tableaux contenant les positions des éléments qui constituent le serpent
    int lesX[TAILLE];
    int lesY[TAILLE];

    // représente la touche frappée par l'utilisateur : touche de direction ou pour l'arrêt
    char touche;

    //direction courante du serpent (HAUT, BAS, GAUCHE ou DROITE)
    char direction = DROITE;

    // le plateau de jeu
    tPlateau lePlateau;

    bool collision = false;
    bool gagne = false;
    bool pommeMangee = false;
   
    // initialisation de la position du serpent : positionnement de la
    // tête en (X_INITIAL, Y_INITIAL), puis des anneaux à sa gauche
    for(int i=0 ; i<TAILLE ; i++)
    {
        lesX[i] = X_INITIAL-i;
        lesY[i] = Y_INITIAL;
    }

    // mise en place du plateau
    initPlateau(lePlateau);
    system("clear");
    dessinerPlateau(lePlateau);

    srand(time(NULL));
    ajouterPomme(lePlateau);

    // initialisation : le serpent se dirige vers la DROITE
    dessinerSerpent(lesX, lesY);
    disable_echo();
    time_t debut = clock();

    // boucle de jeu. Arret si touche STOP, si collision avec une bordure ou
    // si toutes les pommes sont mangées
    do {
        // Trouver la nouvelle direction optimale vers la pomme
        direction = trouverNouvelleDirection(lesX, lesY, direction);
        
        progresser(lesX, lesY, &direction, lePlateau, &collision, &pommeMangee);
        
        if (pommeMangee)
        {
            nbPommes++;
            gagne = (nbPommes == NB_POMMES);
            if (!gagne)
            {
                ajouterPomme(lePlateau);
                pommeMangee = false;
            }	
        }
        
        if (!gagne)
        {
            if (!collision)
            {
                usleep(ATTENTE);
                if (kbhit()==1)
                {
                    touche = getchar();
                }
            }
        }
    } while ( (touche != STOP) && !collision && !gagne);
    
    enable_echo();
    gotoxy(HAUTEUR_PLATEAU+1, 1);
    
    if (gagne)
    {
        clock_t fin = clock();
        gotoxy(LARGEUR_PLATEAU + 2, 1);
        printf("Le serpent s'est déplacé de %d cases.\n", nbDepUnitaires);
        gotoxy(LARGEUR_PLATEAU + 2, 2);
        printf("La partie a durée %.2f secondes.\n", (difftime(fin, debut) / CONVERTION_SECONDE) );
        gotoxy(1, HAUTEUR_PLATEAU+1);
    }
    
    return EXIT_SUCCESS;
}

void initPlateau(tPlateau plateau)
{
    int i, j;
    // initialisation du plateau avec des espaces
    for (i = 1 ; i <= LARGEUR_PLATEAU ; i++)
    {
        for (j = 1 ; j <= HAUTEUR_PLATEAU ; j++)
        {
            plateau[i][j] = VIDE;
        }
    }
    // Mise en place la bordure autour du plateau avec des trous au milieu
    // première ligne
    for (i = 1 ; i <= LARGEUR_PLATEAU ; i++)
    {
        if (i != LARGEUR_PLATEAU/2) plateau[i][1] = BORDURE;
    }
    // côtés gauche et droit
    for (j = 1 ; j <= HAUTEUR_PLATEAU ; j++)
    {
        if (j != HAUTEUR_PLATEAU/2) 
        {
            plateau[1][j] = BORDURE;
            plateau[LARGEUR_PLATEAU][j] = BORDURE;
        }
    }
    // dernière ligne
    for (i = 1; i <= LARGEUR_PLATEAU ; i++)
    {
        if (i != LARGEUR_PLATEAU/2) plateau[i][HAUTEUR_PLATEAU] = BORDURE;
    }
}

void dessinerPlateau(tPlateau plateau)
{
    int i, j;
    // affiche à l'écran le contenu du tableau 2D représentant le plateau
    for (i = 1 ; i <= LARGEUR_PLATEAU ; i++){
        for (j = 1 ; j <= HAUTEUR_PLATEAU ; j++)
        {
            afficher(i, j, plateau[i][j]);
        }
    }
}

void ajouterPomme(tPlateau plateau)
{
    // génère aléatoirement la position d'une pomme,
    // vérifie que ça correspond à une case vide
    // du plateau puis l'ajoute au plateau et l'affiche
    plateau[lesPommesX[nbPommes]][lesPommesY[nbPommes]] = POMME;
    afficher(lesPommesX[nbPommes], lesPommesY[nbPommes], POMME);
}

void afficher(int x, int y, char car)
{
    gotoxy(x, y);
    printf("%c", car);
    gotoxy(1,1);
}

void effacer(int x, int y)
{
    gotoxy(x, y);
    printf(" ");
    gotoxy(1,1);
}

void dessinerSerpent(int lesX[], int lesY[])
{
    int i;
    // affiche les anneaux puis la tête
    for(i = 1 ; i < TAILLE ; i++)
    {
        afficher(lesX[i], lesY[i], CORPS);
    }
    afficher(lesX[0], lesY[0], TETE);
}

bool estDirectionInterdite(char nouvelleDirection, char ancienneDirection)
{
    // Interdire le demi-tour
    if ((ancienneDirection == HAUT && nouvelleDirection == BAS) ||
        (ancienneDirection == BAS && nouvelleDirection == HAUT) ||
        (ancienneDirection == GAUCHE && nouvelleDirection == DROITE) ||
        (ancienneDirection == DROITE && nouvelleDirection == GAUCHE))
    {
        return true;
    }
    return false;
}

char trouverNouvelleDirection(int lesX[], int lesY[], char ancienneDirection)
{
    char nouvelleDirection;

    // Trouver la direction vers la pomme
    if (lesX[0] < lesPommesX[nbPommes])
    {
        nouvelleDirection = DROITE;
    }
    else if (lesX[0] > lesPommesX[nbPommes])
    {
        nouvelleDirection = GAUCHE;
    }
    else if (lesY[0] < lesPommesY[nbPommes])
    {
        nouvelleDirection = BAS;
    }
    else if (lesY[0] > lesPommesY[nbPommes])
    {
        nouvelleDirection = HAUT;
    }
    else
    {
        nouvelleDirection = ancienneDirection;
    }

    // Vérifier si la nouvelle direction n'est pas un demi-tour
    if (estDirectionInterdite(nouvelleDirection, ancienneDirection))
    {
        // Si demi-tour interdit, garder l'ancienne direction
        nouvelleDirection = ancienneDirection;
    }

    return nouvelleDirection;
}

void progresser(int lesX[], int lesY[], char* direction, tPlateau plateau, bool * collision, bool * pomme)
{
    int i;
    // efface le dernier élément avant d'actualiser la position de tous les 
    // éléments du serpent avant de le redessiner
    effacer(lesX[TAILLE-1], lesY[TAILLE-1]);

    // Déplacer les segments du corps
    for(i = TAILLE - 1 ; i > 0 ; i--)
    {
        lesX[i] = lesX[i-1];
        lesY[i] = lesY[i-1];
    }

    // Faire progresser la tête dans la nouvelle direction
    switch(*direction)
    {
        case HAUT   : 
            lesY[0] = (lesY[0] == 1) ? HAUTEUR_PLATEAU : lesY[0] - 1;
            break;
        case BAS    :
            lesY[0] = (lesY[0] == HAUTEUR_PLATEAU) ? 1 : lesY[0] + 1;
            break;
        case DROITE :
            lesX[0] = (lesX[0] == LARGEUR_PLATEAU) ? 1 : lesX[0] + 1;
            break;
        case GAUCHE :
            lesX[0] = (lesX[0] == 1) ? LARGEUR_PLATEAU : lesX[0] - 1;
            break;
    }

    *pomme = false;
    // détection d'une "collision" avec une pomme
    if (plateau[lesX[0]][lesY[0]] == POMME)
    {
        *pomme = true;
        // la pomme disparait du plateau
        plateau[lesX[0]][lesY[0]] = VIDE;
    }
    // détection d'une collision avec le corps du serpent (nouveau critère)
    for (i = 1; i < TAILLE; i++)
    {
        if (lesX[0] == lesX[i] && lesY[0] == lesY[i])
        {
            *collision = true;
            break;
        }
    }
   
    dessinerSerpent(lesX, lesY);
    nbDepUnitaires++;
}

/* Les autres fonctions utilitaires restent identiques à la version précédente */
void gotoxy(int x, int y)
{ 
    printf("\033[%d;%df", y, x);
}

int kbhit()
{
    // la fonction retourne :
    // 1 si un caractere est present
    // 0 si pas de caractere présent
    int unCaractere = 0;
    struct termios oldt, newt;
    int ch;
    int oldf;

    // mettre le terminal en mode non bloquant
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
 
    ch = getchar();

    // restaurer le mode du terminal
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);
 
    if(ch != EOF)
    {
        ungetc(ch, stdin);
        unCaractere = 1;
    } 
    return unCaractere;
}

void enable_echo()
{
    struct termios tty;

    // Obtenir les attributs du terminal
    if (tcgetattr(STDIN_FILENO, &tty) == -1)
	{
        perror("tcgetattr");
        exit(EXIT_FAILURE);
    }

    // Réactiver le flag ECHO
    tty.c_lflag |= ECHO;

    // Appliquer les nouvelles configurations
    if (tcsetattr(STDIN_FILENO, TCSANOW, &tty) == -1)
	{
        perror("tcsetattr");
        exit(EXIT_FAILURE);
    }
}

// Fonction pour désactiver l'echo
void disable_echo()
{
    struct termios tty;

    // Obtenir les attributs du terminal
    if (tcgetattr(STDIN_FILENO, &tty) == -1) 
	{
        perror("tcgetattr");
        exit(EXIT_FAILURE);
    }

    // Désactiver le flag ECHO
    tty.c_lflag &= ~ECHO;

    // Appliquer les nouvelles configurations
    if (tcsetattr(STDIN_FILENO, TCSANOW, &tty) == -1)
	{
        perror("tcsetattr");
        exit(EXIT_FAILURE);
    }
}