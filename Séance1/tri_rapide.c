#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#define TAILLE 300000
typedef int tableau[TAILLE];

int partition(tableau t, int debut, int fin, int pivot);
void triRapide(tableau t, int debut, int fin);
int aleatoire();
void genereTableau(tableau T);
void afficheTableau(tableau T);

int main(){
    time_t debut  = time(NULL);
    tableau t = {};
    genereTableau(t);
    triRapide(t, 5, 1);
    afficheTableau(t);
    time_t fin = time(NULL);
    printf("durée : %3.f secondes\n", difftime(fin, debut));
    return EXIT_SUCCESS;
}

int partition(tableau t, int debut, int fin, int pivot){
    int temp = 0, temp2 = 0, temp3 = 0;
    t[pivot] = temp;
    t[pivot] = t[fin];
    t[fin] = temp;
    int j = debut;
    for (int i = debut ; i < (fin -1) ; i++){
        if (t[i] <= t[fin]){
            t[i] = temp2;
            t[i] = t[fin];
            t[fin] = temp2;
            j++;
        }
    }
    t[fin] = temp3;
    t[fin] = t[j];
    t[j] = temp3;

    return j;
}

void triRapide(tableau t, int debut, int fin){
    int pivot;
    if (debut < fin) {
        pivot = (debut+fin)/2;
        pivot = partition(t, debut, fin, pivot);
        triRapide(t, debut, pivot-1);
        triRapide(t, pivot+1, fin);
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

void afficheTableau(tableau T){
    for (int i = 0 ; i < TAILLE ; i++){
        printf("%d", T[i]);
    }
}