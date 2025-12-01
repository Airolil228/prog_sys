#include "libraries.h"

void usage(char *s){
    fprintf(stderr,"Usage: %s <adresse_ip_serveur> <message_envoyer>\n",s); 
    exit(EXIT_FAILURE); 
}

// Le client PEUT faire bind(), mais c'est souvent inutile
// Le système d'exploitation peut assigner automatiquement une adresse/port

int main(int argc,char *argv[]){  
    char buffer[MAX_BUFFER];
    char *adresse_ip_serveur;
    char *message_envoie;
    int socket_fd;
    struct sockaddr_in servaddr; 
    int nb_octet_reçu;  
    
    int arret = 1; 

    ssize_t nb_oct_envoie;

    if(argc < 3){
        usage(argv[0]);
    }
    
    adresse_ip_serveur =argv[1];
    message_envoie = argv[2]; 
    socklen_t long_servaddr = sizeof(servaddr);

    //Initialisation servaddr
    servaddr.sin_family = AF_INET;
    inet_aton(adresse_ip_serveur,&servaddr.sin_addr);
    servaddr.sin_port = htons(12345); //client doit connaitre .Htons() : prends => 'host byte order', returns => 'network byte order'  
    
    //CREATION DE SOCKET
    if( (socket_fd = socket(AF_INET,SOCK_DGRAM,0)) == -1 ){
        perror("client: socket erreur");
        exit(EXIT_FAILURE);
    } 

    //L'ENVOIE DE MESSAGE AU SERVEUR
    if( (nb_oct_envoie = sendto(socket_fd,
                            message_envoie,
                            strlen(message_envoie),
                            0,
                            (struct sockaddr *)&servaddr,
                            sizeof(servaddr) ) == -1) ){
        perror("client: send erreur");
        close(socket_fd);
        exit(EXIT_FAILURE);
    }

    //RECEVOIR DE LA REPONSE DE LA PART DE SERVEUR 
    if( (nb_octet_reçu = recvfrom(socket_fd,//
                 buffer,
                 MAX_BUFFER-1,
                 0,
                 (struct sockaddr *)&servaddr,
                 &long_servaddr))  == -1){
        perror("client: recvfrom erreur");
        close(socket_fd);
        exit(EXIT_FAILURE);
    }

    buffer[nb_octet_reçu] = '\0';
    fprintf(stdout,"client=>Message reçu: %s\n", buffer);
    
    

    close(socket_fd);
    exit(EXIT_SUCCESS);
}