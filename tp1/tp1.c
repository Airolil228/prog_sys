#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

#define TAILLE_MAT 4

#define CMIN 10
#define CMAX 1000

int matDist[TAILLE_MAT][TAILLE_MAT];
int cModif[TAILLE_MAT][TAILLE_MAT]; 

typedef struct{
    int v1;                                  // sommet de depart
    int v2;                                 // sommet d'arrivee
}Arrete; 

typedef Arrete  * arrete;                    // pointeur sur une arrete
typedef arrete arr;                         // variable globale

arr a;                                        // arrete courante (globale)

void init(){
     a = (arrete) malloc(sizeof(Arrete));
     if( a == NULL) {                                               
        perror("ERREUR: malloc a");
        exit(EXIT_FAILURE);
    }
}



int cout;

struct Solution{
    int k;              // nb arretes  
    int tabA [10][2];  // tableau des arretes
    int w;             // coût 
};

int indTabA = 0; // indice de tabA
typedef struct Solution * Solution; 

struct Solution Meilleur = {TAILLE_MAT,{{0}},0};
struct Solution solPart = {0,{{0}},0};
struct Solution solPartAvecA = {0,{{0}},0};
struct Solution solPartSansA = {0,{{0}},0};

void construireCmodif(){
    memcpy(cModif,matDist,TAILLE_MAT*TAILLE_MAT*sizeof(int));

    // Supprimer toutes les arêtes partant de a->v1
    for (int j = 0; j < TAILLE_MAT; j++) {
        cModif[a->v1][j] = 0;
    }

    // Supprimer toutes les arêtes arrivant à a->v2
    for (int i = 0; i < TAILLE_MAT; i++) {
        cModif[i][a->v2] = 0;
    }

    cModif[a->v2][a->v1] = 0; // on interdit le retour direct
} 

void remplirMat(){
    int i,j; 
    for(i = 0; i < TAILLE_MAT; i++ ){
        for(j = 0; j < TAILLE_MAT; j++){
            if(i == j){
                matDist[i][j] = 0;
            }else{
                matDist[i][j] = ( CMIN + rand() % (CMAX+1));
            } 
        }
    }
}

void afficherMat(int mat[TAILLE_MAT][TAILLE_MAT]){
     int i,j; 
    for(i = 0; i < TAILLE_MAT; i++ ){
        for(j = 0; j < TAILLE_MAT; j++){
            printf("%5d ",mat[i][j]); 
        }
        printf("\n"); 
    }
}

void choisirArrete(Solution solPart){
   int i,j; 
   for(i = 0; i < TAILLE_MAT; i++ ){
       for(j = 0; j < TAILLE_MAT; j++){
          if(cModif[i][j] != 0 ){
              a->v1 = i; 
              a->v2 = j;

              solPart->tabA[indTabA][0] = a->v1; 
              solPart->tabA[indTabA][1] = a->v2;
              printf("choisit l'arrete: (%d,%d)\n\n",solPart->tabA[indTabA][0],solPart->tabA[indTabA][1]); 

              indTabA++;    
              return;
          }
       }
   }
}

void afficherSol(Solution sol){
    int i; 
    printf("k = %d\n",sol->k); 
    for(i = 0; i < sol->k; i++){
        printf("(%d,%d) ",sol->tabA[i][0],sol->tabA[i][1]); 
    }
    printf("\nw = %d\n\n",sol->w); 
}


void  TSP_arrete(Solution  solPart, int c[TAILLE_MAT][TAILLE_MAT]){
    if(solPart->k == Meilleur.k ){                                            //ici Meilleur->k <=> n
        cout = solPart->w; 
        if( cout < Meilleur.w ){                                              // Meilleur->w <=> wb               
            Meilleur.k = solPart->k;                                          // on copie le nb d'aretes
            memcpy(Meilleur.tabA,solPart->tabA,2*10*sizeof(int));             // on copie le tableau des arretes
            Meilleur.w = cout;                                                // on copie le cout
        }
    }else{
        choisirArrete(solPart);                                                // on choisit une arrete a = (v1,v2)
        
        printf("Current values in solPart->tabA:\n");
        for (int i = 0; i < solPart->k + 1; i++) {
            printf("(%d,%d) ", solPart->tabA[i][0], solPart->tabA[i][1]);
        }

        printf("\n");
        solPart->k++;                                                          // on ajoute une arrete

        afficherMat(cModif);                                                   // on affiche la matrice des couts modifiés                        

        //solPartAvecA a = (k, [a1, . . . , ak, a], w + c[i][j]);
        solPartAvecA.k = solPart->k;                                           // on copie le nb d'aretes
        memcpy(solPartAvecA.tabA,solPart->tabA,2*10*sizeof(int));              // on copie le tableau des arretes
        
        solPartAvecA.w = solPart->w+c[a->v1][a->v2];                           // on copie le cout

        construireCmodif();                                                    // on construit la matrice des couts modifiés
        
        TSP_arrete(&solPartAvecA,cModif);                                                                    
        
        //SpartielleSansA = Spartielle;
        solPartSansA.k = solPart->k;                                            // on copie le nb d'aretes
        memcpy(solPartSansA.tabA,solPart->tabA,2*10*sizeof(int));               // on copie le tableau des arretes
        solPartSansA.w = solPart->w;                                            // on copie le cout
        
        TSP_arrete(&solPartSansA,c);
    }
}

int main(int argc, char ** argv){
    srand(time(NULL)); 
    
    init();
    
    remplirMat();

    printf("Matrice des couts:\n");
    afficherMat(matDist);
    printf("_________________________________\n");

    TSP_arrete(&solPart, matDist); 

    printf("\n Meilleure solution:\n");
    afficherSol(&Meilleur);

    free(a);
    exit(EXIT_SUCCESS); 
}