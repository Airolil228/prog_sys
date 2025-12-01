#include "libraries.h"

//SERVEUR 

int main(int argc,char* argv[]){    
    char buffer[MAX_BUFFER]; 
    char *  message = "Salut client !"; 
    
    struct sockaddr_in servaddr,cliaddr;
    int socket_ecoute; 
    socklen_t len_adr_expd; 
    int nb_oct_recu; 
    int arret = 1; 


    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY); //INADDR_ANY => désignant toutes les adresses de la machine. Donc pas besoin de retrouver l’adresse IP de la machine
    servaddr.sin_port = htons(12345); //client doit connaitre .Htons() : prends => 'host byte order', returns => 'network byte order'  
    
    //Initialisation cliaddr
    cliaddr.sin_family = AF_INET;
    cliaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    cliaddr.sin_port = 0; 

    //CREATION DU SOCKET D'UDP
    if( (socket_ecoute = socket(AF_INET,SOCK_DGRAM,0)) == -1){
        perror("serveur->socket erreur");
        exit(EXIT_FAILURE); 
    } 
    
    //BRANCHEMENT DU SOCKET 
    if( bind(socket_ecoute,(struct sockaddr *) &servaddr,sizeof(servaddr)) == -1 ){
        perror("serveur->bind erreur"); 
        close(socket_ecoute); 
        exit(EXIT_FAILURE);
    }
    
    while(arret){
    //RECEPTION DE MESSAGE DEPUIS LE CLIENT 
    len_adr_expd = sizeof(cliaddr); 
    memset(buffer,0,MAX_BUFFER); //Vider le buffeur
    if( (nb_oct_recu = recvfrom(socket_ecoute,buffer,sizeof(buffer),0,(struct sockaddr*)&cliaddr,&len_adr_expd)) == -1 ){
        perror("serveur->recvfrom erreur");
        close(socket_ecoute);
        exit(EXIT_FAILURE); 
    }

    buffer[len_adr_expd] = '\0'; // Fin d'une chaine de car  
    fprintf(stdout,"serveur=>Message reçu: %s\n",buffer); 

    if(strcmp(buffer,"stop") == 0){
        fprintf(stdout,"arret de serveur\n");
        arret = 0; 
    }

    //L'ENVOIE LE MESSAGE AU CLIENT 
    if( sendto(socket_ecoute, 
               message,
               strlen(message), 
               0,
               (struct sockaddr * )&cliaddr,
                sizeof(cliaddr)
             ) == -1 ){
        perror("serveur->sendto erreur");
        close(socket_ecoute);
        exit(EXIT_FAILURE);
    }

    }

    close(socket_ecoute); 
    exit(EXIT_SUCCESS);
}