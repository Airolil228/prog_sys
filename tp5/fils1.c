#include <stdio.h>
#include <stdlib.h>
#include "libraries.h"

int main(int argc, char *argv[],char *envp[]){
    int nb_fils,nb_op,id_shm;
    if(argc < 4){
        fprintf(stderr,"fils: <nb_fils> <nb_op> <idshm> \n"); 
    }

    nb_fils = atoi(argv[1]);
    nb_op = atoi(argv[2]);
    id_shm = atoi(argv[3]);

    //attachement au segment memoire partagé
    int * adr_at = (int *) shmat(id_shm,NULL,0);
    if( adr_at == (int *) -1){
        perror("shmat failed"); 
        exit(EXIT_FAILURE);
    }

    for(int i = 0; i < nb_op; i++){
        *adr_at += 1;
    }
    
    //dettachement du segment memoire partagé
    if(shmdt(adr_at) == -1){
        perror("shmdt failed");
        exit(EXIT_FAILURE); 
    }

    exit(EXIT_SUCCESS);
}