#include "libraries.h"

#define MAX_TAB_FILE 256

int file_circulaire[MAX_TAB_FILE+3];        // indice 0 -> nb elem
                                            // indice 1 -> el premier 
                                            // indice 2 -> el suivant 
                                            // indice n+3 -> le reste des données... 

int premier = 1; 
int prochain = 1; 

void init_file_circulaire(){
    file_circulaire[0] = MAX_TAB_FILE-3;
    file_circulaire[1] = premier; 
    file_circulaire[2] = prochain; 
    for(int i = 3 ; i < MAX_TAB_FILE-3; i++){
        file_circulaire[i] = -1; 
    }
}

void affciher_file_circ(){
    fprintf(stdout,"Nb des elements  : %d\n",file_circulaire[0]);
    fprintf(stdout,"Pointeur 1er el  : %d\n",file_circulaire[1]);
    fprintf(stdout,"Pointeur 2er suivant  : %d\n",file_circulaire[2]); 
    
    for(int i = 3; i < MAX_TAB_FILE-3; i++){
        if( file_circulaire[i] != -1){
            fprintf(stdout," T[%d] : %d", i,file_circulaire[i]); 
        }
     }
}

int est_vide(){
    return (file_circulaire[0] == 0); 
}

void ajout_element(int x ){
     file_circulaire[prochain] = x;
     file_circulaire[0] = file_circulaire[0] + 1; 
     prochain += 1; 

     if( prochain == MAX_TAB_FILE+1){
        prochain = 1; 
     } 
}

void retire_element(){
    file_circulaire[0] = file_circulaire[0] - 1; 
    premier = premier + 1;
 
    if( premier == MAX_TAB_FILE+1 ){
        premier = 1; 
    }
}

int premier_element(){
    return file_circulaire[premier]; 
}

int main(){
    init_file_circulaire();
    affciher_file_circ();

    exit(EXIT_SUCCESS);
}
