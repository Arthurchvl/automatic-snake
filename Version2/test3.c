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

/* Fichiers inclus */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <time.h>
#include <math.h>


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
#define ATTENTE 20000
// caractères pour représenter le serpent
#define CORPS 'X'
#define TETE 'O'
// touches de direction ou d'arrêt du jeu
#define HAUT 3
#define BAS 4
#define GAUCHE 1
#define DROITE 2
#define STOP 'a'
// caractères pour les éléments du plateau
#define BORDURE '#'
#define VIDE ' '
#define POMME '6'
// nombre servant a la convertion du temps en seconde
#define CONVERTION_SECONDE 1000


// définition d'un type pour le plateau : tPlateau
// Attention, pour que les indices du tableau 2D (qui commencent à 0) coincident
// avec les coordonées à l'écran (qui commencent à 1), on ajoute 1 aux dimensions
// et on neutralise la ligne 0 et la colonne 0 du tableau 2D (elles ne sont jamais
// utilisées)
int xPomme[10] = {75, 75, 78, 2, 8, 78, 74,  2, 72, 5};
int yPomme[10] = { 8, 39,  2, 2, 5, 39, 33, 38, 35, 2};
typedef char tPlateau[LARGEUR_PLATEAU+1][HAUTEUR_PLATEAU+1];
// compteur de pommes mangées
int nbPommes = 0;
int nbDepUnitaires = 0;

/* Déclaration des fonctions et procédures (les prototypes) */
void initPlateau(tPlateau plateau);
void dessinerPlateau(tPlateau plateau);
void ajouterPomme(tPlateau plateau);
void afficher(int, int, char);
void effacer(int x, int y);
void dessinerSerpent(int lesX[], int lesY[]);
void progresser(int lesX[], int lesY[], char direction, tPlateau plateau, bool * collision, bool * pomme);
int distance(int x1, int y1, int x2, int y2);
int calculerDist(int teteX, int teteY, int pommeX, int pommeY);
void gotoxy(int x, int y);
int kbhit();
void disable_echo();
void enable_echo();


int main()
{
	time_t debut = clock();

	// 2 tableaux contenant les positions des éléments qui constituent le serpent
    int lesX[TAILLE];
	int lesY[TAILLE];

	//direction courante du serpent (HAUT, BAS, GAUCHE ou DROITE)
	char direction;
	char directionTemp;

	//représente la touche frappée par l'utilisateur
	char touche;

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
	direction = DROITE;

	// boucle principale du jeu. Arrêt si touche STOP (a), si collision avec une bordure ou
	// si toutes les pommes sont mangées
	do {
		// conditions de changements de direction du serpent
		if (lesX[0] < xPomme[nbPommes]) {
				direction = DROITE;
		} else if (lesX[0] > xPomme[nbPommes]) {
				direction = GAUCHE;
		} else if (lesY[0] < yPomme[nbPommes]) {
				direction = BAS;
		} else if (lesY[0] > yPomme[nbPommes]) {
				direction = HAUT;
		}

		// boucle permettant au serpent d'emprunter une issue
		// si le chemin par celle ci est plus court
		int distMin = 0;
		distMin = calculerDist(lesX[0], lesY[0], xPomme[nbPommes], yPomme[nbPommes]);
		switch (distMin){
			case GAUCHE:
			if (lesY[0] == HAUTEUR_PLATEAU/2){
				direction = GAUCHE;
			}
			break;
			case DROITE :
			if (lesY[0] == HAUTEUR_PLATEAU/2){
				direction = DROITE;
			}
			break;
			case HAUT :
			if (lesX[0] == LARGEUR_PLATEAU/2){
				direction = HAUT;
			}
			break;
			case BAS :
			if (lesX[0] == LARGEUR_PLATEAU/2){
				direction = BAS;
			}
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

				if (lesX[0] == xPomme[nbPommes])
				{
					if ((direction == HAUT) || (direction == BAS))
					{
						directionTemp = direction;
						if (xPomme[nbPommes] > 20)
						{
							direction = GAUCHE;
							progresser(lesX, lesY, direction, lePlateau, &collision, &pommeMangee);
							direction = (directionTemp = HAUT) ? BAS : HAUT;
							for (int i = 0 ; i < TAILLE ; i++){
								progresser(lesX, lesY, direction, lePlateau, &collision, &pommeMangee);
							}
						} else {
							direction = DROITE;
							progresser(lesX, lesY, direction, lePlateau, &collision, &pommeMangee);
							for (int i = 0; i < TAILLE ; i++)
							{
								direction = (directionTemp = HAUT) ? BAS : HAUT;
								progresser(lesX, lesY, direction, lePlateau, &collision, &pommeMangee);
							}
						}
					}
				} else if ((lesX[0] == 2) && (lesY[0] == 2)){
					direction = (direction == GAUCHE)? BAS : DROITE;
					progresser(lesX, lesY, direction, lePlateau, &collision, &pommeMangee);
				}
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
	gotoxy(1, HAUTEUR_PLATEAU+1);
	if (gagne)
	{
		clock_t fin = clock();
		enable_echo();
		gotoxy(1, HAUTEUR_PLATEAU+1);
		printf("Votre serpent s'est déplacé %d fois\n", nbDepUnitaires);
		printf("La partie a durée %.2f seconde \n", (difftime(fin, debut) / CONVERTION_SECONDE) );
	}
	return EXIT_SUCCESS;
}


/************************************************/
/*		FONCTIONS ET PROCEDURES DU JEU 			*/
/************************************************/
void initPlateau(tPlateau plateau)
{
	int i, j;

	for (i = 1 ; i <= LARGEUR_PLATEAU ; i++)
	{
		for (int j=1 ; j <= HAUTEUR_PLATEAU ; j++)
		{
			plateau[i][j] = VIDE;
		}
	}
	
	for (i = 1 ; i <= LARGEUR_PLATEAU ; i++)
	{
		plateau[i][1] = BORDURE;
	}
	
	for (j = 1 ; j <= HAUTEUR_PLATEAU ; j++)
	{
			plateau[1][j] = BORDURE;
			plateau[LARGEUR_PLATEAU][j] = BORDURE;
		}
	
	for (i = 1; i <= LARGEUR_PLATEAU ; i++)
	{
		plateau[i][HAUTEUR_PLATEAU] = BORDURE;
	}
	plateau[LARGEUR_PLATEAU][HAUTEUR_PLATEAU/2] = VIDE;
	plateau[LARGEUR_PLATEAU/2][HAUTEUR_PLATEAU] = VIDE;
	plateau[LARGEUR_PLATEAU/2][1] = VIDE;
	plateau[1][HAUTEUR_PLATEAU/2] = VIDE;

}

void dessinerPlateau(tPlateau plateau){
	// affiche eà l'écran le contenu du tableau 2D représentant le plateau
	for (int i=1 ; i<=LARGEUR_PLATEAU ; i++){
		for (int j=1 ; j<=HAUTEUR_PLATEAU ; j++){
			afficher(i, j, plateau[i][j]);
		}
	}
}

void ajouterPomme(tPlateau plateau)
{
	// génère aléatoirement la position d'une pomme,
	// vérifie que ça correspond à une case vide
	// du plateau puis l'ajoute au plateau et l'affiche
	plateau[xPomme[nbPommes]][yPomme[nbPommes]] = POMME;
	afficher(xPomme[nbPommes], yPomme[nbPommes], POMME);
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
	afficher(lesX[0], lesY[0],TETE);
	for(i = 1 ; i < TAILLE ; i++)
	{
		afficher(lesX[i], lesY[i], CORPS);
	}
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
			lesY[0]--;
			break;
		case BAS	:
			lesY[0]++;
			break;
		case DROITE	:
			lesX[0]++;
			break;
		case GAUCHE	:
			lesX[0]--;
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

	// gestion de l'utilisation des issues
	if (lesX[0] == 0 && lesY[0] == HAUTEUR_PLATEAU / 2) 
	{
		lesX[0] = LARGEUR_PLATEAU - 2;
	} else if (lesX[0] == LARGEUR_PLATEAU - 1 && lesY[0] == HAUTEUR_PLATEAU / 2){
		lesX[0] = 1;
	} else if (lesY[0] == 0 && lesX[0] == LARGEUR_PLATEAU / 2) {
		lesY[0] = HAUTEUR_PLATEAU - 2;
	} else if (lesY[0] == HAUTEUR_PLATEAU && lesX[0] == LARGEUR_PLATEAU / 2){
		lesY[0] = 2;
	}

	// Vérification des collisions avec le corps du serpent
    for (int i = 1; i < TAILLE; i++) {
        if ((lesX[0] == lesX[i]) && (lesY[0] == lesY[i])) {
            *collision = true;
        }
    }

   	dessinerSerpent(lesX, lesY);
	nbDepUnitaires++;
}

int distance(int x1, int y1, int x2, int y2) {
    return sqrt(((abs(x1 - x2)) * (abs(x1 - x2))) + ((abs(y1 - y2))*abs(y1 - y2))) ;
}

int calculerDist(int teteX, int teteY, int pommeX, int pommeY) {
	int tabDistance[5] = {0, 0, 0, 0, 0};

    int distDirecte = distance(teteX, teteY, pommeX, pommeY);
	tabDistance[0] = distDirecte;
    
    int distGauche = distance(1, HAUTEUR_PLATEAU / 2, teteX, teteY) 
                         + distance(LARGEUR_PLATEAU - 1, HAUTEUR_PLATEAU / 2, pommeX, pommeY);
	tabDistance[1] = distGauche;

    int distDroite = distance(LARGEUR_PLATEAU - 1, HAUTEUR_PLATEAU / 2, teteX, teteY) 
                         + distance(1, HAUTEUR_PLATEAU / 2, pommeX, pommeY);
	tabDistance[2] = distDroite;

    int distHaut = distance(LARGEUR_PLATEAU / 2, 1, teteX, teteY)
                    + distance(LARGEUR_PLATEAU / 2, HAUTEUR_PLATEAU - 1, pommeX, pommeY); 
	tabDistance[3] = distHaut;

    int distBas = distance(LARGEUR_PLATEAU / 2, HAUTEUR_PLATEAU - 1, teteX, teteY)
                    + distance(LARGEUR_PLATEAU / 2, 1, pommeX, pommeY);
	tabDistance[4] = distBas;

	int distMin = 0;
	int min = tabDistance[0];
    for (int i = 1 ; i < 5 ; i++)
	{
		if (min >= tabDistance[i])
		{
			min = tabDistance[i];
			distMin = i;
		}
	}

    return distMin;
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