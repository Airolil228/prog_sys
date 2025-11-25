#include "libraries.h"

#define MAX_SHM 256
#define MAX_TAB 256

void usage(char * s){
    fprintf(stdout,"<usage> %s: <taille_sg> <nb_consom> <nb_prod>\n",s); 
    exit(EXIT_FAILURE); 
}

int main(int argc, char *argv[]){ 
    int taille_sg,nb_consom,nb_prod; 
    int id_shm;
    key_t cle;
    pid_t pid, pid_t[MAX_TAB];   

    if(argc < 4 ){
        usage(argv[0]); 
    }

    taille_sg =  atoi(argv[1]); 
    nb_consom =  atoi(argv[2]);
    nb_prod   =  atoi(argv[3]);

    FILE * fic = fopen("fichier","a"); 
    if(fic == NULL){
        fprintf(stderr,"Erreur d'ouverture de fichiers \n"); 
        fclose(fic);
        exit(EXIT_FAILURE);  
    }
    
    //Génération du cle
    if( (cle = ftok("fic",'A')) == -1){
        perror("ftok erreur"); 
        fclose(fic);
        exit(EXIT_FAILURE); 
    } 
    
    //Génération du segment mémoire partagé
    if( (id_shm = shmget(cle,(size_t) MAX_SHM, IPC_CREAT | 0666)) == -1){
        perror("shmget erreur ");
        fclose(fic);
        exit(EXIT_FAILURE);
    }

    pid = fork(); 

    if( pid == -1 ){
        perror("fork erreur");
        shmctl(id_shm,IPC_RMID,NULL);
        fclose(fic); 
        exit(EXIT_FAILURE);  
    }
    
    /*
    if( pid == 0){
        for(int i = 0; i < nb_consom; i++){

        }

        for(int i = 0; i < nb_prod; i++){

        }
    }
    */



    fclose(fic); 
    shmctl(id_shm,IPC_RMID,NULL);
    exit(EXIT_SUCCESS);
}