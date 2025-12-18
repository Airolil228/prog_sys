#include "librairies.h"

int sigusr1recu = 0; 

void arret(int sig){
    printf("signal %d (TERM) reçu \n", sig); 
    sigusr1recu++;
}

void usage(char * appel){
    fprintf(stdout,"Usage : %s <nombre vendeurs> <nombre caissiers> <nombre clients> <num vendeur> <clef file de message> \n", appel);
    exit(EXIT_FAILURE);
}

int main(int argc, char* argv[]){
    if (argc != 6){
        usage(argv[0]);
    }
    int num_vendeur = atoi(argv[4]);
    int rayon_competence;
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
    if (nb_rayon_inoccupe > 0){
        rayon_competence = tab_rayon_inoccupe[rand() % nb_rayon_inoccupe];
        nb_rayon_inoccupe --;
    }else{
        if (nb_rayon_inoccupe == 0){
            rayon_competence = rand() % NB_RAYON;  // 0 à NB_RAYON - 1
        }
    }


    struct sigaction sa;

    sa.sa_handler = arret;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;


    if (sigaction(SIGUSR1, &sa, NULL) < 0) {
        perror("Erreur sigaction");
        exit(1);
    }

    while (!sigusr1recu){
        reception = msgrcv(msgid, &msg, sizeof(msg), 0, 0);
        
        if (reception == -1){
            perror("Erreur msgrcv");
            break;
        }

        // Traitement du message
        printf("Reçu: %s\n", msg);

        if (!(rayon_competence = msg->rayon)){
            // Répondre pas mon rayon + id d'un autre vendeur
            msg->mtype = msg->client_id; // Reponse vers le client
            //msg->vendeur_reco = 0;  // Vendeur à recommander à déterminer
            envoi = msgsnd(msgid, &msg, sizeof(msg),0);

            if (envoi == -1){
                perror("Erreur msgsnd");

                // Retour à l'attente d'un autre client
            }
        }else{
            // tire un temps aléatoire
            // sleep(temps);
            // reveil le client (message)

            // Décision du client :
            // Attendre la réponse msgrcv
            // Si vente refusée -> fin 
            // Si vente accepté ->
            // Tirer montant aléatoire
            // Envoyer aux caissiers

            // Fin avec ce client retour étape 1
        }
    }

    exit(EXIT_SUCCESS);
}