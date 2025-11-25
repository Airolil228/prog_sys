#include "libraries.h"

#define TAILLE_SHM 1024 
#define MAX_CHAINE 100

void usage(char * nom_prog){
    printf("usage %s : <nb_pid_fils> <nb_op> \n",nom_prog); 
}

int main(int argc, char *argv[]){
    int id_shm,nb_fils,nb_op;
    int * adr_at = NULL;
    int semid; 
    char str_nbfils[MAX_CHAINE], str_nbop[MAX_CHAINE], str_idshm[MAX_CHAINE], str_semid[MAX_CHAINE],str_idfils[MAX_CHAINE];
    pid_t pid;
    struct sembuf semaph; 
    
    if(argc < 3){
        usage(argv[0]);
        exit(EXIT_FAILURE); 
    }
    
    nb_fils = atoi(argv[1]);
    nb_op = atoi(argv[2]);  
    pid_t pids[nb_fils]; 

    FILE * fichier = fopen("fichier","a"); 
    if (fichier == NULL) {
        perror("fopen failed");
        exit(EXIT_FAILURE);
    }

    // Générer une clé unique pour la mémoire partagée
    key_t cle = ftok("fichier", 'A');
    
    if(cle == -1){
        perror("ftok failed");
        fclose(fichier);
        exit(EXIT_FAILURE);
    }

    // Créer la mémoire partagée
    if( ( id_shm = shmget(cle,(size_t) TAILLE_SHM,IPC_CREAT | 0666)) == -1 ){
        perror("shmget failed");
        fclose(fichier);
        exit(EXIT_FAILURE);
    }

    // Attacher la mémoire partagée
    if ((adr_at = (int *) shmat(id_shm, NULL, 0)) == (int *) -1) {  
        perror("shmat failed");
        shmctl(id_shm, IPC_RMID, NULL);
        fclose(fichier);
        exit(EXIT_FAILURE);
    }

    // Initialiser l'entier partagé à 5
    *adr_at = 5;
    fprintf(stdout, "Entier partagé: %d\n", *adr_at); 

    //Creation de semaphore 
    if( (semid = semget(cle, 1, IPC_CREAT | 0666)) == -1){
        perror("semget erreur: ");
        shmctl(id_shm,IPC_RMID,NULL);
        fclose(fichier);
        exit(EXIT_FAILURE);
    }

    //Prépare le semaphore pour la synchronisation 
    if( semctl(semid,0,SETVAL,1) == -1 ){
        perror("semctl erreur: ");
        shmctl(id_shm,IPC_RMID, NULL);
        semctl(semid,0,IPC_RMID);
        fclose(fichier);  
        exit(EXIT_FAILURE);
    }


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

        sprintf(str_nbfils, "%d", nb_fils); // Conversion des entiers en chaînes de caractères
        sprintf(str_nbop  , "%d", nb_op); // Conversion des entiers en chaînes de caractères
        sprintf(str_idshm , "%d", id_shm); // Conversion des entiers en chaînes de caractères
        sprintf(str_semid , "%d", semid);
        sprintf(str_idfils, "%d", i+1);
        char * args[] = {"./fils2", str_nbfils, str_nbop, str_idshm,str_semid,str_idfils, NULL}; // Préparation des arguments pour execv
        
        if (pid == 0) {
            // Code exécuté par le processus fils
            //printf("processus %d, PID %d démarre\n",i+1,getpid());
            
            if( execv("./fils2", args) == -1 ) {
                perror("execl failed");
                exit(EXIT_FAILURE);
            }
            
            printf("Fin de processus fils %d, PID %d \n",i+1,getpid());
            exit(EXIT_FAILURE);
        }else{
            // Code exécuté par le processus père
            pids[i] = pid; // Stocker le PID du fils
        }
    }

    for(int i = 0; i < nb_fils; i++){
        // Attendre la fin de chaque processus fils
        waitpid(pids[i], NULL, 0);
    }
    fprintf(stdout,"Valeur de l'entier à la fin : %d\n",*adr_at);
    fprintf(stdout,"Valeur attendue             : %d\n", nb_op*( (nb_fils*(nb_fils+1))/2) );
    
    shmdt(adr_at); //liberer l'adresse
    shmctl(id_shm,IPC_RMID,NULL); // nettoyage de IPC (mémoire partagé) 
    semctl(semid,0,IPC_RMID); // nettoyage de IPC (semaphore)
    fclose(fichier);
    exit(EXIT_SUCCESS);
}