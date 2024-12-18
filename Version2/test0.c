/**
 * @file version2.c
 * @brief automatisation d'un jeu snake créé lors de la SAE 1.01
 * @author Chauvel Arthur, Le Chevère Yannis
 * @version 2.0
 * @date 04/12/2024
 *
 * étape 2 de l'automatisation d'un jeu snake codé en C lors de la SAE 1.01
 * le jeu initialise le plateau aux dimensions données en constantes,
 * il initialise le serpent qux coordonées de départ données en paramètres,
 * les milieux du serpent sont percées pour créer des issues 
 * que le serpent peut emprunter en cours de partie
 * pour se téléporter à l'issue de la bordure opposée.
 * Le jeu utilise une touche d'arrêt si le joueur veut mettre fin à la partie sans perdre.
 * La collision de la tête du serpent avec une bordure 
 * ou avec le corps du serpent met fin à la partie.
 * En cas de victoire, le jeu affiche le nombre de déplacement unitaires du serpent
 * et le temps CPU du programme.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <time.h>

// Game configuration
#define TAILLE 10              // Snake length
#define LARGEUR_PLATEAU 80     // Board width
#define HAUTEUR_PLATEAU 40     // Board height
#define X_INITIAL 40           // Initial snake head x-position
#define Y_INITIAL 20           // Initial snake head y-position
#define NB_POMMES 10           // Number of apples to eat to win
#define ATTENTE 200000         // Movement delay (microseconds)
#define CONVERTION_SECONDE 1000 // Time conversion factor

// Game characters
#define CORPS 'X'   // Snake body
#define TETE 'O'    // Snake head
#define BORDURE '#' // Border
#define VIDE ' '    // Empty space
#define POMME '6'   // Apple

// Directions
#define HAUT 'z'
#define BAS 's'
#define GAUCHE 'q'
#define DROITE 'd'
#define STOP 'a'

// Predefined apple positions
int lesPommesX[NB_POMMES] = {75, 75, 78, 2, 8, 78, 74, 2, 72, 5};
int lesPommesY[NB_POMMES] = {8, 39, 2, 2, 5, 39, 33, 38, 35, 2};

// Type definition for game board
typedef char tPlateau[LARGEUR_PLATEAU+1][HAUTEUR_PLATEAU+1];

// Global variables
int nbPommes = 0;
int nbDepUnitaires = 0;

// Function prototypes
void initPlateau(tPlateau plateau);
void dessinerPlateau(tPlateau plateau);
void ajouterPomme(tPlateau plateau);
void afficher(int x, int y, char car);
void effacer(int x, int y);
void dessinerSerpent(int lesX[], int lesY[]);
void progresser(int lesX[], int lesY[], char direction, tPlateau plateau, bool *collision, bool *pomme);
void gotoxy(int x, int y);
int kbhit();
void disable_echo();
void enable_echo();
bool estDeplacementValide(int lesX[], int lesY[], char nouvelleDirection, char directionPrecedente);

int main()
{
    time_t debut = clock();

    int lesX[TAILLE];
    int lesY[TAILLE];

    char direction;
    char directionPrecedente;
    char touche = 0;

    tPlateau lePlateau;

    bool collision = false;
    bool gagne = false;
    bool pommeMangee = false;
   
    // Initialize snake position
    for(int i = 0 ; i < TAILLE ; i++)
    {
        lesX[i] = X_INITIAL - i;
        lesY[i] = Y_INITIAL;
    }

    // Setup game board
    initPlateau(lePlateau);
    system("clear");
    dessinerPlateau(lePlateau);

    srand(time(NULL));
    ajouterPomme(lePlateau);

    dessinerSerpent(lesX, lesY);
    disable_echo();
    direction = DROITE;
    directionPrecedente = DROITE;

    // Main game loop
    do {
		
        // Try to move towards the current apple
        if (lesX[0] < lesPommesX[nbPommes] && estDeplacementValide(lesX, lesY, DROITE, directionPrecedente)) {
            direction = DROITE;
        } else if (lesX[0] > lesPommesX[nbPommes] && estDeplacementValide(lesX, lesY, GAUCHE, directionPrecedente)) {
            direction = GAUCHE;
        } else if (lesY[0] < lesPommesY[nbPommes] && estDeplacementValide(lesX, lesY, BAS, directionPrecedente)) {
            direction = BAS;
        } else if (lesY[0] > lesPommesY[nbPommes] && estDeplacementValide(lesX, lesY, HAUT, directionPrecedente)) {
            direction = HAUT;
        }

        progresser(lesX, lesY, direction, lePlateau, &collision, &pommeMangee);
        
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
                if (kbhit() == 1)
                {
                    touche = getchar();
                }
            }
        }
        directionPrecedente = direction;
    } while ((touche != STOP) && !collision && !gagne);

    enable_echo();
    gotoxy(1, HAUTEUR_PLATEAU + 1);
    
    if (gagne)
    {
        clock_t fin = clock();
        printf("Votre serpent s'est déplacé %d fois\n", nbDepUnitaires);
        printf("La partie a durée %.2f seconde\n", (difftime(fin, debut) / CONVERTION_SECONDE));
    }
    return EXIT_SUCCESS;
}

// New function to prevent snake from turning back on itself
bool estDeplacementValide(int lesX[], int lesY[], char nouvelleDirection, char directionPrecedente)
{
    // Cannot turn back 180 degrees
    if ((nouvelleDirection == DROITE && directionPrecedente == GAUCHE) ||
        (nouvelleDirection == GAUCHE && directionPrecedente == DROITE) ||
        (nouvelleDirection == HAUT && directionPrecedente == BAS) ||
        (nouvelleDirection == BAS && directionPrecedente == HAUT)) {
        return false;
    }
    
    return true;
}


/************************************************/
/*		FONCTIONS ET PROCEDURES DU JEU 			*/
/************************************************/
void initPlateau(tPlateau plateau)
{
	int i, j;
	// initialisation du plateau avec des espaces
	for (i = 1 ; i <= LARGEUR_PLATEAU ; i++)
	{
		for (int j=1 ; j <= HAUTEUR_PLATEAU ; j++)
		{
			plateau[i][j] = VIDE;
		}
	}
	// Mise en place la bordure autour du plateau
	// première ligne
	for (i = 1 ; i <= LARGEUR_PLATEAU ; i++)
	{
		plateau[i][1] = BORDURE;
	}
	// lignes intermédiaires
	for (j = 1 ; j <= HAUTEUR_PLATEAU ; j++)
	{
			plateau[1][j] = BORDURE;
			plateau[LARGEUR_PLATEAU][j] = BORDURE;
		}
	// dernière ligne
	for (i = 1; i <= LARGEUR_PLATEAU ; i++)
	{
		plateau[i][HAUTEUR_PLATEAU] = BORDURE;
	}
	plateau[LARGEUR_PLATEAU][HAUTEUR_PLATEAU/2] = VIDE;
	plateau[LARGEUR_PLATEAU/2][HAUTEUR_PLATEAU] = VIDE;
	plateau[LARGEUR_PLATEAU/2][1] = VIDE;
	plateau[1][HAUTEUR_PLATEAU/2] = VIDE;

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
	// génère la position d'une pomme aux coordonnées définies,
	//plateau[lesPommesX[nbPommes]][lesPommesY[nbPommes]] = POMME;
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
	afficher(lesX[0], lesY[0],TETE);
}

void progresser(int lesX[], int lesY[], char direction, tPlateau plateau, bool * collision, bool * pomme)
{
	int i;
	// efface le dernier élément avant d'actualiser la position de tous les 
	// élémentds du serpent avant de le  redessiner et détecte une
	// collision avec une pomme ou avec une bordure
	effacer(lesX[TAILLE-1], lesY[TAILLE-1]);

	for(i = TAILLE - 1 ; i > 0 ; i--)
	{
		lesX[i] = lesX[i-1];
		lesY[i] = lesY[i-1];
	}
	//faire progresser la tete dans la nouvelle direction
	switch(direction)
	{
		case HAUT	: 
			lesY[0] = lesY[0] - 1;
			break;
		case BAS	:
			lesY[0] = lesY[0] + 1;
			break;
		case DROITE	:
			lesX[0] = lesX[0] + 1;
			break;
		case GAUCHE	:
			lesX[0] = lesX[0] - 1;
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
	// détection d'une collision avec la bordure
	else if (plateau[lesX[0]][lesY[0]] == BORDURE)
	{
		*collision = true;
	}

	if (lesX[0] == 0 && lesY[0] == HAUTEUR_PLATEAU / 2) lesX[0] = LARGEUR_PLATEAU - 2;
    else if (lesX[0] == LARGEUR_PLATEAU - 1 && lesY[0] == HAUTEUR_PLATEAU / 2) lesX[0] = 1;
    else if (lesY[0] == 0 && lesX[0] == LARGEUR_PLATEAU / 2) lesY[0] = HAUTEUR_PLATEAU - 2;
    else if (lesY[0] == HAUTEUR_PLATEAU - 1 && lesX[0] == LARGEUR_PLATEAU / 2) lesY[0] = 1;

	    // Vérification des collisions avec le corps du serpent
    for (int i = 1; i < TAILLE; i++) {
        if ((lesX[0] == lesX[i]) && (lesY[0] == lesY[i])) {
            *collision = true;
        }
    }

   	dessinerSerpent(lesX, lesY);
	nbDepUnitaires++;
}



/************************************************/
/*				 FONCTIONS UTILITAIRES 			*/
/************************************************/
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

// Fonction pour réactiver l'echo
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