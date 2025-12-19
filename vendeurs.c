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
    magasin m;
    m.nb_vendeurs = atoi(argv[1]);
    m.nb_caissiers = atoi(argv[2]);
    m.nb_clients = atoi(argv[3])+1;

    key_t key = (key_t) atoi(argv[5]);
    struct message msg;
    ssize_t reception;
    int envoi;
    int vendeur_recommande;
    int temps;
    int x,i;

    struct sigaction sa;

    sa.sa_handler = arret;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    srand(time(NULL));

    //fprintf(stderr,"Debut vendeur.c : <nombre vendeurs : %d > <nombre caissiers : %d > <nombre clients : %d > <num vendeur : %d > <clef file de message : %d > \n",atoi(argv[1]),atoi(argv[2]),atoi(argv[3]),atoi(argv[4]),atoi(argv[5]));
    
    /* Récupération du segment de mémoire partagée */
    int shmid = shmget(key, sizeof(magasin), 0666);
    if (shmid == -1){
        perror("Erreur shmget (vendeur)");
        return 1;
    }

    /* Attachement du segment au processus */
    magasin *shm_ptr = shmat(shmid, NULL, 0);
    if (shm_ptr == (magasin*)-1){
        perror("Erreur shmat (vendeur)");
        return 1;
    }

    /* Recup de la file de messages */
    int msgid = msgget(key, 0666 | IPC_CREAT);
    if (msgid == -1){
        perror("Erreur msgget");
        return 1;
    }

    //recupère le sémaphore
    int sem_id = semget(key,1,0666);
    if (sem_id == -1) {
        perror("semget vendeur");
        exit(1);
    }

    //P(sem_id,SEM_VENDEURS);
    if (shm_ptr->nb_rayon_inoccupe > 0){
        x = rand() % shm_ptr->nb_rayon_inoccupe;
        shm_ptr->tab_vendeurs[num_vendeur].rayon_expertise = shm_ptr->tab_rayon_inoccupe[x];
        for (i=x;i<(shm_ptr->nb_rayon_inoccupe-1);i++){
            shm_ptr->tab_rayon_inoccupe[i] = shm_ptr->tab_rayon_inoccupe[i+1];
        }
        shm_ptr->nb_rayon_inoccupe--;
    }else{
        if (shm_ptr->nb_rayon_inoccupe == 0){
            shm_ptr->tab_vendeurs[num_vendeur].rayon_expertise = rand() % NB_RAYON;  // 0 à NB_RAYON - 1
        }
    }
    //V(sem_id,SEM_VENDEURS);


    if (sigaction(SIGUSR2, &sa, NULL) < 0) {
        perror("Erreur sigaction");
        exit(1);
    }

    while (!sigusr2recu){

        fprintf(stderr,"Ligne 88 : Le vendeur %d attend une requete d'un client \n",num_vendeur);
        reception = msgrcv(msgid, &msg, sizeof(struct message) - sizeof(long), VENDEUR_BASE + num_vendeur, 0);
        
        if (reception == -1){
            perror("Erreur msgrcv (coté vendeur), Ligne 92 (reception de la requete du client)");
            break;
        }

        // Traitement du message
        fprintf(stderr,"Ligne 97 : Le vendeur %d recoit une requete du client %d \n",num_vendeur, msg.client_id);
        shm_ptr->tab_vendeurs[num_vendeur].nb_clients_attente++;

        if (shm_ptr->tab_vendeurs[num_vendeur].rayon_expertise != msg.rayon){
            // Répondre pas mon rayon + id d'un autre vendeur

            msg.mtype = CLIENT_DISCUSSION_BASE + msg.client_id; // Reponse vers le client
            vendeur_recommande = (num_vendeur + 1) % m.nb_vendeurs;
            /*if (vendeur_recommande >= m.nb_vendeurs + VENDEUR_BASE || vendeur_recommande < VENDEUR_BASE){ // NB_vendeur > tout num vendeur car num vendeur va de 0 à NB_VENDEUR - 1 normalement
                vendeur_recommande = VENDEUR_BASE;
            }*/
            msg.vendeur_reco = vendeur_recommande;  // Vendeur à recommander à déterminer
            fprintf(stderr,"Ligne 109 : Le vendeur %d a redirigé le client %d vers le vendeur %d\n",num_vendeur, msg.client_id, vendeur_recommande);
            envoi = msgsnd(msgid, &msg, sizeof(struct message) - sizeof(long),0);

            if (envoi == -1){
                perror("Erreur msgsnd (coté vendeur), Ligne 113 (envoi du vendeur recommandé)");
            }
            shm_ptr->tab_vendeurs[num_vendeur].etat = LIBRE;
            // Retour à l'attente d'un autre client -> Fin du tour
        }else{
            // tire un temps aléatoire
            temps = rand() % 5 + 1; // Temps de 1 à 5 sec
            
            // Le vendeur prend en charge le client
            shm_ptr->tab_vendeurs[num_vendeur].etat = OCCUPE;
            shm_ptr->tab_vendeurs[num_vendeur].client_actuel = msg.client_id;
            fprintf(stderr,"Ligne 124 : Le vendeur %d dors %d secondes\n",num_vendeur, temps);
            sleep(temps);

            // reveil le client (message)
            msg.mtype = CLIENT_DISCUSSION_BASE + msg.client_id; // Reponse vers le client
            msg.vendeur_reco = num_vendeur; // Permet de bien montrer qu'on reste avec le meme vendeur
            fprintf(stderr,"Ligne 130 : Le vendeur %d répond au client %d qu'il est le specialiste de ce rayon %d\n",num_vendeur, msg.client_id, msg.rayon);

            envoi = msgsnd(msgid, &msg, sizeof(struct message) - sizeof(long),0);

            if (envoi == -1){
                perror("Erreur msgsnd (coté serveur), Ligne 135 (envoi du reveil de client)");
                exit(EXIT_FAILURE);
            }

            // Décision du client :
            // Attendre la réponse msgrcv
            reception = msgrcv(msgid, &msg, sizeof(struct message) - sizeof(long), VENDEUR_DISCUSSION_BASE + num_vendeur, 0);

            if (reception == -1){
                perror("Erreur msgrcv (coté serveur), Ligne 144 (attente de la décision du client)");
                exit(EXIT_FAILURE);
            }

            if (msg.decision == 0){
                // Si vente refusée -> fin::
                shm_ptr->tab_vendeurs[num_vendeur].etat = LIBRE;
            }else{

                // Si vente accepté ->
                // Tirer montant aléatoire
                msg.montant = rand() % (MONTANT_MAX - MONTANT_MIN + 1) + MONTANT_MIN;
                // Envoyer aux caissiers
                msg.mtype = 10000; // type commun pour les caissiers
                msg.vendeur_reco = num_vendeur; // Permet de bien montrer qu'on reste avec le meme vendeur
                envoi = msgsnd(msgid, &msg, sizeof(struct message) - sizeof(long),0);
                shm_ptr->tab_vendeurs[num_vendeur].etat = LIBRE;
                    
                if (envoi == -1){
                    perror("Erreur msgsnd (coté serveur), Ligne 163 (envoi du message au caissier pour se préparer à recevoir le client)");
                    exit(EXIT_FAILURE);
                }
            }
            // Fin avec ce client retour étape 1
        }
    }

    shmdt(shm_ptr);
    exit(EXIT_SUCCESS);
}