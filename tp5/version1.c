#include "libraries.h"

#define TAILLE_SHM 1024 

void usage(char * nom_prog){
    printf("usage %s : <nb_pid_fils> <nb_op> \n",nom_prog); 
}

int main(int argc, char *argv[]){
    int id_shm,nb_fils,nb_op;
    int * adr_at = NULL;

    pid_t pid; 

    if(argc < 3){
        usage(argv[0]);
        exit(EXIT_FAILURE); 
    }
    
    nb_fils = atoi(argv[1]);
    nb_op = atoi(argv[2]);  

    FILE * fichier = fopen("fichier","a"); 
    if (fichier == NULL) {
        perror("fopen failed");
        exit(EXIT_FAILURE);
    }
    key_t cle = ftok("fichier", 'A');
    
    if(cle == -1){
        perror("ftok failed");
        fclose(fichier);
        exit(EXIT_FAILURE);
    }
    
    if( ( id_shm = shmget(cle,(size_t) TAILLE_SHM,IPC_CREAT | 0666)) == -1 ){
        perror("shmget failed");
        fclose(fichier);
        exit(EXIT_FAILURE);
    }

    
    if ((adr_at = (int *) shmat(id_shm, NULL, 0)) == (int *) -1) {  
        perror("shmat failed");
        shmctl(id_shm, IPC_RMID, NULL);
        fclose(fichier);
        exit(EXIT_FAILURE);
    }

    *adr_at = 5;
    fprintf(stdout, "Entier partagé: %d\n", *adr_at); 

     // Boucle pour créer n processus fils
    fprintf(stdout,"Lancement des fils.............fait\n"); 
     for(int i = 0; i < nb_fils ; i++){
        // Création d'un nouveau processus
        pid = fork();

        if(pid == -1){
            // Erreur lors de la création du processus
            perror("Erreur de fork");
            exit(EXIT_FAILURE);


        }

        sprintf(str_nbfils, "%d", nb_fils);
        sprintf(str_nbop  , "%d", nb_op);
        sprintf(str_idshm , "%d", id_shm);

        char * args[] = {"./fils", str_nbfils, str_nbop, str_idshm, NULL};
        
        if (pid == 0) {
            // Code exécuté par le processus fils
            //printf("processus %d, PID %d démarre\n",i+1,getpid());
            
            char * str_nbfils[10], str_nbop[10], str_idshm[10];    

            if( execv("./fils", args) == -1 ) {
                perror("execl failed");
                exit(EXIT_FAILURE);
            }
            
            printf("Fin de processus fils %d, PID %d \n",i+1,getpid());
            exit(EXIT_FAILURE);
        }else{
            // Code exécuté par le processus père
            /*pere  */
        }
    }
    
    fprintf(stdout,"Valeur attendue             : %d\n", nb_op*( (nb_fils*(nb_fils+1))/2) );
    fflush(stdout);
    
    shmdt(adr_at); //liberer l'adresse
    shmctl(id_shm,IPC_RMID,NULL); // nettoyage de IPC 
    fclose(fichier);
    exit(EXIT_SUCCESS);
}