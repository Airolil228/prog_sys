#include "../libraries.h" 

int sigusr1recu = 0; 

void habitude(int sig){
    printf("%d signal USR1 reçu \n", sig); 
    sigusr1recu++;
}

void arret(int sig){
    fprintf(stderr,"Père reçu le SIGINT signale, arret , avec le nb SIGUSR1 %d \n",sigusr1recu);
    exit(EXIT_SUCCESS); 
}

void mon_action(int signal, void (*handler)(int)){
    struct sigaction sa;

    sa.sa_handler = handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    sigaction(signal, &sa, NULL);
}

int main(int argc, char * argv[]){
    pid_t pid; 
    int n = atoi(argv[1]); 
    int encore = 1; 
    int i;
    int sig_recu = 0;
    int sig_envoye = 0;    
    sigset_t ensvide; 
    
    if(n < 1){
        fprintf(stderr,"usage : %s nombre_processus \n",argv[0]); 
    }

    mon_action(SIGUSR1,habitude); 
    mon_action(SIGINT,arret); 

    pid = fork();
    printf("Fils lancée avec le PID %d \n", getpid());
        
    if( pid < -1){
        fprintf(stderr,"Erreur de création de processus \n");  
        exit(EXIT_FAILURE);
    }else if( pid == 0 ){
        printf("[%d] j'envoie des signaux \n", getpid());
        
        for(i = 0 ; i < n ; i++){
            sig_envoye++; 
            printf("envoi du signal \n");
            kill(getppid(),SIGUSR1);
            sigsuspend(&ensvide); 
        } 
        
    }

    wgile(1){

    }
    
    fprintf(stderr,"\nTerminè, \n");
    fprintf(stderr,"sigusr1 envoye: %d ",sig_envoye);         
    fprintf(stderr," sigusr1 acquitte: %d ",sigusr1recu);
    
    exit(EXIT_SUCCESS); 
}