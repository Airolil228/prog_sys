#include "librairies.h"

int sigusr1recu = 0; 

void debut(int sig){
    fprintf(stderr,"Le signal %d commence (dans Initial.c) \n", sig); 
    sigusr1recu++;
}

int sigusr2recu = 0; 

void arret(int sig){
    fprintf(stderr,"signal %d (TERM) reçu (clients) \n", sig); 
    sigusr2recu++;
}

void usage(char * appel){
    fprintf(stdout,"Usage : %s <nombre vendeurs> <nombre caissiers> <nombre clients> <num client> <clef file de message> <fic_log> \n", appel);
    exit(EXIT_FAILURE);
}

int main(int argc, char* argv[]){
    if (argc != 7){
        usage(argv[0]);
    }
    fflush(stderr);
    fprintf(stderr,"TEST\n");
    int num_client = atoi(argv[4]);
    magasin m;
    m.nb_vendeurs = atoi(argv[1]);
    m.nb_caissiers = atoi(argv[2]);
    m.nb_clients = atoi(argv[3])+1;

    key_t key = (key_t) atoi(argv[5]);

    int fic_log = atoi(argv[6]);

    struct message msg;
    ssize_t reception;
    int envoi;

    /* Recup de la file de messages */
    int msgid = msgget(key, 0666 | IPC_CREAT);

    if (msgid == -1){
        perror("Erreur msgget");
        return 1;
    }

    srand(time(NULL) ^ getpid());

    int num_rayon = rand() % NB_RAYON; // Entre 0 et 9 

    int num_vendeur = rand() % (m.nb_vendeurs); //Ou on peut aussi récuperer le vendeur le moins occupé 
    int num_caissier = rand() % (m.nb_caissiers); //Ou on peut aussi récuperer le vendeur le moins occupé 


    struct sigaction sa;

    sa.sa_handler = debut;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;


    if (sigaction(SIGUSR1, &sa, NULL) < 0) {
        perror("Erreur sigaction");
        exit(1);
    }

    while (!sigusr1recu){
        pause();
    }

    struct sigaction sa2;

    sa2.sa_handler = arret;
    sigemptyset(&sa2.sa_mask);
    sa2.sa_flags = 0;


    if (sigaction(SIGUSR2, &sa2, NULL) < 0) {
        perror("Erreur sigaction");
        exit(1);
    }

    while (!sigusr2recu){
        msg.vendeur_reco = 0;
        msg.decision = 0;
        msg.montant = 0;
        msg.num_caissier = 0; 
        msg.type_message = 0; 

        fprintf(stderr,"Ligne %d :Le client %d veut acheter au rayon %d au près du vendeur %d \n", __LINE__,num_client,num_rayon,num_vendeur);
        dprintf(fic_log,"Ligne %d :Le client %d veut acheter au rayon %d au près du vendeur %d \n", __LINE__,num_client,num_rayon,num_vendeur);

        // Il prend le vendeur désigné et lui fait savoir son rayon
        msg.mtype = VENDEUR_BASE + num_vendeur;   // vendeur ciblé
        msg.client_id = num_client;
        msg.rayon = num_rayon;
        msg.num_vendeur = num_vendeur;

        envoi = msgsnd(msgid, &msg, sizeof(struct message) - sizeof(long),0);

        if (envoi == -1){
            fprintf(stderr,"Erreur msgsnd (coté client), Ligne %d (Premiere requete client à ce vendeur) \n", __LINE__);
            perror("msgsnd");
            exit(EXIT_FAILURE);
        }

        // En attente de la réponse du vendeur
        reception = msgrcv(msgid, &msg, sizeof(struct message) - sizeof(long), CLIENT_DISCUSSION_BASE + num_client, 0);
        
        if (reception == -1){
            if (errno == EINTR) continue; // signal → on reprend
            fprintf(stderr,"Erreur msgrcv (coté client), Ligne 79 (Premiere reponse vendeur : specialisation du rayon) \n");
            perror("msgrcv");
            exit(EXIT_FAILURE);
        }

        if (msg.vendeur_reco != num_vendeur){
            num_vendeur = msg.vendeur_reco;
            fprintf(stderr,"Ligne 90 : Le client %d change de vendeur, il va vers : %d \n",num_client,num_vendeur);
            dprintf(fic_log,"Ligne 90 : Le client %d change de vendeur, il va vers : %d \n",num_client,num_vendeur);
            // Fin pour ce tour, le vendeur n'est pas le bon.
            continue;
        }else{

            fprintf(stderr,"Ligne 94 : Le client %d a trouvé le bon vendeur : %d \n",num_client,num_vendeur);
            dprintf(fic_log,"Ligne 94 : Le client %d a trouvé le bon vendeur : %d \n",num_client,num_vendeur);
            // Le vendeur passe un certain temps avant de repondre puis redeclenche la procédure
            /*reception = msgrcv(msgid, &msg, sizeof(struct message) - sizeof(long), CLIENT_DISCUSSION_BASE + num_client, 0);
        
            if (reception == -1){
                perror("Erreur msgrcv (coté client), Ligne 94 (Le vendeur a attendu puis a envoyé sa réponse)");
                exit(EXIT_FAILURE);
            }*/

            // Decision 1 ou 0
            if ((rand() % 101) > PROBA_ACHAT){
                msg.decision = 1;
            }else{
                msg.decision = 0;
            }
            msg.mtype = VENDEUR_DISCUSSION_BASE + num_vendeur;
            envoi = msgsnd(msgid, &msg, sizeof(struct message) - sizeof(long),0);

            if (envoi == -1){
                fprintf(stderr,"Erreur msgsnd (coté client), Ligne %d (Rendu de la décision du client) \n", __LINE__);
                dprintf(fic_log,"Erreur msgsnd (coté client), Ligne %d (Rendu de la décision du client) \n", __LINE__);
                perror("msgsnd");
                exit(EXIT_FAILURE);
            }


            if (msg.decision == 0){
                fprintf(stderr,"Ligne 118 : Le client %d refuse l'achat, et pars\n",num_client);
                dprintf(fic_log,"Ligne 118 : Le client %d refuse l'achat, et pars\n",num_client);
                break; // abandon
            }else{
                // S'occuper avec le caissier
                fprintf(stderr,"Ligne 122 : Le client %d va à la caisse.\n",num_client);
                dprintf(fic_log,"Ligne 122 : Le client %d va à la caisse.\n",num_client);

                //break; // va a la caisse

                msg.mtype = CAISSIER_DISCUSSION_BASE + num_caissier;
                msg.client_id = num_client;

                envoi = msgsnd(msgid, &msg, sizeof(struct message) - sizeof(long),0);

                if (envoi == -1){
                    fprintf(stderr,"Erreur msgsnd (coté client), Ligne %d (Envoie du num de client au caissier) \n", __LINE__);
                    dprintf(fic_log,"Erreur msgsnd (coté client), Ligne %d (Envoie du num de client au caissier) \n", __LINE__);
                    perror("msgsnd");
                    exit(EXIT_FAILURE);
                }

                // En attente de la réponse du caissier
                reception = msgrcv(msgid, &msg, sizeof(struct message) - sizeof(long), CLIENT_DISCUSSION_BASE + num_client, 0);
                
                if (reception == -1){
                    if (errno == EINTR) continue; // signal → on reprend
                    fprintf(stderr,"Erreur msgrcv (coté client), Ligne 79 (Premiere reponse caissier : prix) \n");
                    dprintf(fic_log,"Erreur msgrcv (coté client), Ligne 79 (Premiere reponse caissier : prix) \n");
                    perror("msgrcv");
                    exit(EXIT_FAILURE);
                }


                // Le caissier conclu l'achat
                reception = msgrcv(msgid, &msg, sizeof(struct message) - sizeof(long), CLIENT_DISCUSSION_BASE + num_client, 0);
                
                if (reception == -1){
                    if (errno == EINTR) continue; // signal → on reprend
                    fprintf(stderr,"Erreur msgrcv (coté client), Ligne 79 (Le caissier conclu l'achat) \n");
                    dprintf(fic_log,"Erreur msgrcv (coté client), Ligne 79 (Le caissier conclu l'achat) \n");
                    perror("msgrcv");
                    exit(EXIT_FAILURE);
                }

                fprintf(stderr,"Le client %d fini, il a acheté %d dihram au rayon %d.\n",num_client,msg.montant,num_rayon);
                dprintf(fic_log,"Le client %d fini, il a acheté %d dihram au rayon %d.\n",num_client,msg.montant,num_rayon);

            }

        }
    }

    fprintf(stderr, "Fin du client %d \n",getpid());
    dprintf(fic_log, "Fin du client %d \n",getpid());
    close(fic_log);
    exit(EXIT_SUCCESS);
}