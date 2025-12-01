#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>

// Déclaration du gestionnaire de signal
void handler(int sig) {
    printf("\nCtrl+C reçu (signal %d). Arrêt du processus.\n", sig);
    exit(0);
}

int main(int argc,char *argv[]){
    int i,n;
    pid_t pid ; 
    
    if(argc < 2){
        printf("Usage: %s arg1 \n",argv[0]);
        exit(EXIT_FAILURE);
    }

    signal(SIGINT,handler);
    n = atoi(argv[1]);

    // Boucle pour créer n processus fils
    for(i = 0; i < n ; i++){
        // Création d'un nouveau processus
        pid = fork();

        if(pid == -1){
            // Erreur lors de la création du processus
            perror("Erreur de fork");
            exit(EXIT_FAILURE);
        } else if (pid == 0) {
            // Code exécuté par le processus fils
            printf("processus %d, PID %d démarre\n",i+1,getpid()); 

            while(1) {
                pause();  // Bloque le fils jusqu'à signal
            }
        
            printf("Fin de processus fils %d, PID %d \n",i+1,getpid());
            exit(EXIT_FAILURE);
        }else{
            // Code exécuté par le processus père
            /*pere  */
        }
    }

    printf("Plus de création de processus. Processus père PID %d en attente...\n", getpid()); 
    printf("Appuyez sur Ctrl+C pour terminer tous les processus.\n");

    while(1) {
        pause();  // Bloque le père jusqu'à signal
    }
    
    exit(EXIT_SUCCESS);
}
