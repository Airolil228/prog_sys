#include "librairies.h"

int sigusr1recu = 0; 

void debut(int sig){
    fprintf(stderr,"Le signal %d commence (dans Initial.c) \n", sig); 
    sigusr1recu++;
}

int sigusr2recu = 0; 

void arret(int sig){
    fprintf(stderr,"signal %d (TERM) reçu (Caissiers) \n", getpid()); 
    sigusr2recu++;
}

void usage(char * appel){
    fprintf(stdout,"Usage : %s <nombre vendeurs> <nombre caissiers> <nombre clients> <num caissier> <clef file de message> \n", appel);
    exit(EXIT_FAILURE);
}


int main(int argc, char* argv[]){
    if (argc != 6){
        usage(argv[0]);
    }
    fflush(stderr);
    int num_caissier = atoi(argv[4]);
    magasin m;
    m.nb_vendeurs = atoi(argv[1]);
    m.nb_caissiers = atoi(argv[2]);
    m.nb_clients = atoi(argv[3])+1;

    key_t key = (key_t)atoi(argv[5]);
    struct message msg;
    ssize_t reception;
    int temps; //variable aleatoire
    


    struct sigaction sa;

    sa.sa_handler = debut;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;

    struct sigaction sa2;

    sa2.sa_handler = arret;
    sigemptyset(&sa2.sa_mask);
    sa2.sa_flags = 0;


    if (sigaction(SIGUSR1, &sa, NULL) < 0) {
        perror("Erreur sigaction");
        exit(1);
    }

    while (!sigusr1recu){
        pause();
    }

    srand(time(NULL));

    //recupère le sémaphore
    int sem_id = semget(key,5,0666);

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
    
    if (sigaction(SIGUSR2, &sa2, NULL) < 0) {
        perror("Erreur sigaction");
        exit(1);
    }

    while (!sigusr2recu){
        //Attendre un client
        fprintf(stderr, "Caissier %d attend un client...\n",num_caissier);
        
        reception = msgrcv(msgid, &msg, sizeof(struct message) - sizeof(long), 10000, 0);

        if(reception == -1 && errno != EINTR){
            perror("Erreur msgrcv (cassier)");
        }

        //Mettre à jour l'état 
        //P(sem_id,SEM_CAISSIERS); // LOCK
        shm_ptr->tab_caissiers[num_caissier].nb_clients_attente++;
        shm_ptr->tab_caissiers[num_caissier].etat = OCCUPE;
        shm_ptr->tab_caissiers[num_caissier].client_actuel = msg.client_id;
        //V(sem_id,SEM_CAISSIERS); // UNLOCK

        //Récuperer le montant 
        int montant = msg.montant; 
        fprintf(stdout,"Caissier %d: Client %d doit payer %d...\n",
            num_caissier,
            msg.client_id,
            montant);

        //Communiquer le prix au client 
        msg.mtype = CLIENT_DISCUSSION_BASE + msg.client_id;
        int envoi = msgsnd(msgid,&msg,sizeof(struct message) - sizeof(long),0);

        if(envoi == -1){
            perror("Erreur msgsnd (cassier -> cleint)");
            exit(EXIT_FAILURE);
        }

        //Temps de paiement alétoire 
        temps = rand() % 5+1; // 1 à 5 secondes (ajouster )
        sleep(temps); 

        //Cassier terminé: Remettre à LIBRE
        fprintf(stdout,"Caissier %d a terminé avec le client %d\n", 
                num_caissier, 
                msg.client_id
        );
        
        //Remettre à LIBRE
        //P(sem_id,SEM_CAISSIERS);// LOCK
        shm_ptr->tab_caissiers[num_caissier].etat = LIBRE;
        shm_ptr->tab_caissiers[num_caissier].client_actuel = -1;
        shm_ptr->tab_caissiers[num_caissier].nb_clients_attente--;
        //V(sem_id,SEM_CAISSIERS);// UNLOCK 
    }
    fprintf(stderr, "Fin du caissier %d \n",getpid());
    
    exit(EXIT_SUCCESS);
}