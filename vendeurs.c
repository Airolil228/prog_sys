#include "librairies.h"

int sigusr2recu = 0; 

void arret(int sig){
    printf("signal %d (TERM) reçu (vendeurs) \n", sig); 
    sigusr2recu++;
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
    struct message msg;
    ssize_t reception;
    int envoi;
    int vendeur_recommande;
    int temps;

    struct sigaction sa;

    sa.sa_handler = arret;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    srand(time(NULL));

    //fprintf(stderr,"Debut vendeur.c : <nombre vendeurs : %d > <nombre caissiers : %d > <nombre clients : %d > <num vendeur : %d > <clef file de message : %d > \n",atoi(argv[1]),atoi(argv[2]),atoi(argv[3]),atoi(argv[4]),atoi(argv[5]));
    

    /* Recup de la file de messages */
    int msgid = msgget(key, 0666 | IPC_CREAT);
    if (msgid == -1){
        perror("Erreur msgget");
        return 1;
    }

    if (nb_rayon_inoccupe > 0){
        rayon_competence = tab_rayon_inoccupe[rand() % nb_rayon_inoccupe];
        nb_rayon_inoccupe --;
    }else{
        if (nb_rayon_inoccupe == 0){
            rayon_competence = rand() % NB_RAYON;  // 0 à NB_RAYON - 1
        }
    }


    if (sigaction(SIGUSR2, &sa, NULL) < 0) {
        perror("Erreur sigaction");
        exit(1);
    }

    while (!sigusr2recu){

        fprintf(stderr,"Le vendeur %d attend une requete d'un client \n",num_vendeur);
        reception = msgrcv(msgid, &msg, sizeof(struct message) - sizeof(long), VENDEUR_BASE + num_vendeur, 0);
        
        if (reception == -1){
            perror("Erreur msgrcv (coté vendeur), Ligne 70 (reception de la requete du client)");
            break;
        }

        // Traitement du message
        fprintf(stderr,"Le vendeur %d recoit une requete d'un client \n",num_vendeur);
        if (rayon_competence != msg.rayon){
            // Répondre pas mon rayon + id d'un autre vendeur

            msg.mtype = CLIENT_BASE + msg.client_id; // Reponse vers le client
            vendeur_recommande = (num_vendeur + 1) % m.nb_vendeurs;
            /*if (vendeur_recommande >= m.nb_vendeurs + VENDEUR_BASE || vendeur_recommande < VENDEUR_BASE){ // NB_vendeur > tout num vendeur car num vendeur va de 0 à NB_VENDEUR - 1 normalement
                vendeur_recommande = VENDEUR_BASE;
            }*/
            msg.vendeur_reco = vendeur_recommande;  // Vendeur à recommander à déterminer

            envoi = msgsnd(msgid, &msg, sizeof(struct message) - sizeof(long),0);

            if (envoi == -1){
                perror("Erreur msgsnd (coté vendeur), Ligne 87 (envoi du vendeur recommandé)");
            }
            fprintf(stderr,"Le vendeur %d a redirigé le client %d vers le vendeur %d\n",num_vendeur, msg.client_id, vendeur_recommande);
            // Retour à l'attente d'un autre client -> Fin du tour
        }else{
            // tire un temps aléatoire
            temps = rand() % 5 + 1; // Temps de 1 à 5 sec
            sleep(temps);

            // reveil le client (message)
            msg.mtype = CLIENT_BASE + msg.client_id; // Reponse vers le client
            msg.vendeur_reco = num_vendeur; // Permet de bien montrer qu'on reste avec le meme vendeur

            envoi = msgsnd(msgid, &msg, sizeof(struct message) - sizeof(long),0);

            if (envoi == -1){
                perror("Erreur msgsnd (coté serveur), Ligne 102 (envoi du reveil de client)");
                exit(EXIT_FAILURE);
            }

            // Décision du client :
            // Attendre la réponse msgrcv
            reception = msgrcv(msgid, &msg, sizeof(struct message) - sizeof(long), VENDEUR_BASE + num_vendeur, 0);

            if (reception == -1){
                perror("Erreur msgrcv (coté serveur), Ligne 111 (attente de la décision du client)");
                exit(EXIT_FAILURE);
            }

            if (msg.decision == 0){
                // Si vente refusée -> fin
            }else{
                // Si vente accepté ->
                // Tirer montant aléatoire
                msg.montant = rand() % (MONTANT_MAX - MONTANT_MIN + 1) + MONTANT_MIN;
                // Envoyer aux caissiers
                msg.mtype = 10000; // type commun pour les caissiers

                envoi = msgsnd(msgid, &msg, sizeof(struct message) - sizeof(long),0);

                if (envoi == -1){
                    perror("Erreur msgsnd (coté serveur), Ligne 126 (envoi du message au caissier pour se préparer à recevoir le client)");
                    exit(EXIT_FAILURE);
                }
            }
            // Fin avec ce client retour étape 1
        }
    }

    exit(EXIT_SUCCESS);
}