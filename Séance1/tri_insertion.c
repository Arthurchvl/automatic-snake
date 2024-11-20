#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#define TAILLE 300000
typedef int tableau[TAILLE];

void tri_insertion(tableau T);
void affichetableau(tableau T);
int aleatoire();
void genereTableau(tableau T);

int main(){
    time_t debut = time(NULL);
    tableau T = {};
    genereTableau(T);
    tri_insertion(T);
    affichetableau(T);
    time_t fin = time(NULL);
    printf("durée : %3.f secondes\n", difftime(fin, debut));
    return EXIT_SUCCESS;
}

void tri_insertion(tableau T){
    int x, j = 0;
    for (int i = 1 ; i <= (TAILLE - 1) ; i++){
        x = T[i];
        j = i;
        while ((j > 0) && (T[j - 1] > x )){
            T[j] = T[j-1];
            j-- ;
        }
        T[j] = x ;
    }
}

void affichetableau(tableau T){
    for (int i = 0 ; i < TAILLE ; i++){
        printf("%d", T[i]);
    }
}

int aleatoire(){
    srand(time(NULL));
    int x = rand();
    return x;
}

void genereTableau(tableau T){
    for (int i = 0; i < TAILLE ; i++){
        T[i] = aleatoire();
    }
}