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
    int v1; 
    int v2; 
}Arrete; 

typedef Arrete  * arrete;
arrete a; 

typedef struct{
    int k;        // nb arretes  
    int tabA [2][10];  // 
    int w;        // coût 
}Solution; 



int cout; 







void construireCmodif(arrete a){
    for(int i = 0; i < TAILLE_MAT; i++ ){
        for(int j = 0; j < TAILLE_MAT; j++){
            if( i == a->v1 ){
                matDist[a->v1][j] = 0;
            }
            if( j == a->v2){
                matDist[i][a->v2] = 0; 
            }
            
            
        }
    }
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

void afficherMat(){
     int i,j; 
    for(i = 0; i < TAILLE_MAT; i++ ){
        for(j = 0; j < TAILLE_MAT; j++){
            printf("%5d ",matDist[i][j]); 
        }
        printf("\n"); 
    }
}

void choisirArrete(arrete  a){
   int i,j; 
   for(i = 0; i < TAILLE_MAT; i++ ){
       for(j = 0; j < TAILLE_MAT; j++){
          if(matDist[i][j] != 0 ){
              a->v1 = i; 
              a->v2 = j;
              printf("choisit l'arrete: (%d,%d)\n",i,j); 
              return ;
          }
            
       }
   }
}


int * TSP_arrete(Solution * solPart, int c[TAILLE_MAT][TAILLE_MAT],Solution * Meilleur){
  
    if(solPart->k == Meilleur->k ){ //ici Meilleur->k <=> n
        cout = solPart->w; 
        if( cout < Meilleur->w ){  //  Meilleur->w <=> wb
            Meilleur->k = solPart->k; 
            
            Meilleur->k = cout; 
        }
    }else{
        choisirArrete(a);
        solPart->k = solPart->k++;
        
        memcpy(cModif,c,2*TAILLE_MAT*sizeof(int));
        solPart->w += c[a->v1][a->v2];
        
        construireCmodif(a);
        TSP_arrete(solPart,cModif,Meilleur);
        
    }
}

int main(int argc, char ** argv){
    srand(time(NULL));
    
    Solution * solPart  =  (Solution *) malloc(sizeof(Solution));
    Solution * Meilleur =  (Solution *) malloc(sizeof(Solution));
    
   
    
    arrete a = (arrete) malloc(sizeof(Arrete));
    
    
    remplirMat();
    afficherMat();
    TSP_arrete(solPart, matDist,Meilleur); 

    free(a);
    free(solPart);
    
    exit(EXIT_SUCCESS); 
}