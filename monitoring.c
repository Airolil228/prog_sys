#include "librairies.h"

int arret_monitoring = 0;

void signal_arret(int sig){
    fprintf(stderr, "\nMonitoring Signal %d reçu, arrêt du monitoring...\n", sig);
    arret_monitoring = 1;
}

void usage(char *prog){
    fprintf(stderr, "Usage: %s <clé IPC>\n", prog);
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]){
    if(argc < 2){
        usage(argv[0]);
    }

    key_t key = (key_t)atoi(argv[1]);

    struct sigaction sa;
    sa.sa_handler = signal_arret;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    
    if (sigaction(SIGINT, &sa, NULL) < 0) {
        perror("Erreur sigaction SIGINT");
        exit(EXIT_FAILURE);
    }
    
    if (sigaction(SIGTERM, &sa, NULL) < 0) {
        perror("Erreur sigaction SIGTERM");
        exit(EXIT_FAILURE);
    }
    
    // Récupération de la mémoire partagée
    int shmid = shmget(key, sizeof(magasin), 0666);
    if (shmid == -1) {
        perror("Erreur shmget");
        fprintf(stderr, "Vérifiez que le processus initial est lancé.\n");
        exit(EXIT_FAILURE);
    }

    magasin *shm = shmat(shmid, NULL, SHM_RDONLY);
    if (shm == (magasin *)-1) {
        perror("Erreur shmat");
        exit(EXIT_FAILURE);
    }

    fprintf(stderr, "Monitoring Démarré\n\n");

    while(!arret_monitoring){
        printf("========== MONITORING DU MAGASIN ==========\n\n");
        
        printf("--- VENDEURS ---\n");
        for (int i = 0; i < shm->nb_vendeurs; i++) {
            printf("Vendeur %d (Rayon %d): %d (ETAT) | File: %d | Client: %d\n",
                   i,
                   shm->tab_vendeurs[i].rayon_expertise,
                   shm->tab_vendeurs[i].etat, 
                   shm->tab_vendeurs[i].nb_clients_attente,
                   shm->tab_vendeurs[i].client_actuel);
        }
        
        printf("\n===========================================\n");
        
        sleep(1);
    }

    shmdt(shm);
    fprintf(stderr, "Monitoring Terminé.\n");
    exit(EXIT_SUCCESS);
}