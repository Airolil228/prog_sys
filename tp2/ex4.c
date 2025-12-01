#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <time.h> 
#include <wait.h>

#define BORNE_MIN 20
#define BORNE_MAX 30


int main(int argc,char **argv){
    srand(time(NULL)); 
    int i;
    int n = atoi(argv[1]);
    pid_t pid; 
    int random,encore; 

    for(i = 0; i < n ; i++){
        pid = fork(); 
        if(pid < 0){
            perror("Erreur de fork");
            exit(EXIT_FAILURE); 
        }else if(pid == 0){
            srand(time(NULL) + getpid());
            random = rand() % (BORNE_MAX - BORNE_MIN + 1) + BORNE_MIN;      // lancemment du temps alatoire  
            
            printf("Lancement du fils %d\n", getpid()); 
            printf("[%d] je m’endors pendant %d secondes.\n",getpid(),random);      
            
            sleep(random);
            exit(EXIT_SUCCESS);
        }else{
            /*Processus père */
        }
    }

    fprintf(stderr,"Attente de leur terminaison");
    
    encore = 1;
    while (encore){
        if ((waitpid(-1,NULL,0)==-1) && (errno == ECHILD)){
            encore = 0;
        }else{
            fprintf(stdout,".");
        }
    }

    printf("Plus de fils,bye \n");
    exit(EXIT_SUCCESS);
}