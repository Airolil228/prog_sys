#include "librairies.h"

int sigusr1recu = 0; 

void arret(int sig){
    printf("signal %d (TERM) reçu \n", sig); 
    sigusr1recu++;
}

void usage(char * appel){
    fprintf(stdout,"Usage : %s <nombre vendeurs> <nombre caissiers> <nombre clients> <num client> <clef file de message> \n", appel);
    exit(EXIT_FAILURE);
}

int main(int argc, char* argv[]){
    if (argc != 6){
        usage(argv[0]);
    }
    int num_client = atoi(argv[4]);
    magasin m;
    m.nb_vendeurs = atoi(argv[1]);
    m.nb_caissiers = atoi(argv[2]);
    m.nb_clients = atoi(argv[3])+1;

    key_t key = (key_t)atoi(argv[5]);

    struct msgbuf *msg;
    ssize_t reception;
    int envoi;

    /* Recup de la file de messages */
    int msgid = msgget(key, 0666 | IPC_CREAT);
    if (msgid == -1){
        perror("Erreur msgget");
        return 1;
    }

    srand(time(NULL));

    int num_rayon = rand() % NB_RAYON; // Entre 0 et 9 

    int num_vendeur = rand() % m.nb_vendeurs; // Entre 0 et nb_vendeur -1   /   Ou on peut aussi récuperer le vendeur le moins occupé 
    
    struct sigaction sa;

    sa.sa_handler = arret;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;


    if (sigaction(SIGUSR1, &sa, NULL) < 0) {
        perror("Erreur sigaction");
        exit(1);
    }

    while (!sigusr1recu){
        // Il prend le vendeur désigné et lui fait savoir son rayon
        msg->mtype = num_vendeur;   // vendeur ciblé
        msg->client_id = num_client;
        msg->rayon = num_rayon;
        envoi = msgsnd(msgid, &msg, sizeof(msg),0);

        if (envoi == -1){
            perror("Erreur msgsnd");
            exit(EXIT_FAILURE);
        }

        // En attente de la réponse du vendeur
        reception = msgrcv(msgid, &msg, sizeof(msg), num_vendeur, 0);
        
    }

    exit(EXIT_SUCCESS);
}