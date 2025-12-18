#include "librairies.h"

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


    if (sigaction(SIGUSR1, &action, NULL) < 0) {
        perror("Erreur sigaction");
        exit(1);
    }

    while (1){
        pause();
    }

    exit(EXIT_SUCCESS);
}