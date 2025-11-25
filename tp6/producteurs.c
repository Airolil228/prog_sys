#include "libraries.h"

void arret(int s){
    fprintf(stdout,"Reception de %d...arret de programme \n",s); 
    exit(EXIT_SUCCESS);
} 


void mon_sigaction(int signal, void (*func)(int)){
    struct sigaction action; 
    
    action.sa_handler = func;
    sigemptyset(&action.sa_mask);  
    action.sa_flags = 0; 
    sigaction(signal,&action,NULL);
}

int main(){
    
    fprintf(stdout,"Lancemment du producteur avec pid: %d\n",getpid());     

    exit(EXIT_SUCCESS); 
}