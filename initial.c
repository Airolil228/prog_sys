#include "librairies.h"

int sigtermrecu = 0; 

void arret(int sig){
    printf("signal %d (TERM) reçu \n", sig); 
    sigtermrecu++;
}

void usage(char * appel){
    fprintf(stdout,"Usage : %s <nombre vendeurs> <nombre caissiers> <nombre clients> \n", appel);
    exit(EXIT_FAILURE);
}

int main(int argc, char* argv[]){
    if (argc != 4){
        usage(argv[0]);
    }
    int i,j; int attente;
    magasin m;
    m.nb_vendeurs = atoi(argv[1]);
    m.nb_caissiers = atoi(argv[2]);
    m.nb_clients = atoi(argv[3])+1;

    char num_creation[NB_CHAR_EXEC];
    char str_nb_vendeurs[NB_CHAR_EXEC];
    char str_nb_caissiers[NB_CHAR_EXEC];
    char str_nb_clients[NB_CHAR_EXEC];
    key_t key; char str_key[NB_CHAR_EXEC];
    int msgid;

    struct sigaction sa;

    sa.sa_handler = arret;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;


    // Initialisation des rayons inoccupés pour s'assurer qu'ils sont tous occupés
    nb_rayon_inoccupe = NB_RAYON;
    for (i=0;i<NB_RAYON;i++){
        tab_rayon_inoccupe[i] = i;
    }

    sprintf(str_nb_vendeurs,  "%d", m.nb_vendeurs);  // Conversion des entiers en chaînes de caractères
    sprintf(str_nb_caissiers, "%d", m.nb_caissiers);
    sprintf(str_nb_clients, "%d", m.nb_clients);

    // Créer processus vendeur, caissier et clients

    if(m.nb_vendeurs < NB_RAYON){
        fprintf(stderr,"nombre_vendeurs > nombre de rayon : %d \n",NB_RAYON);
        usage(argv[0]);
    }

    if(m.nb_caissiers < 1){
        fprintf(stderr,"usage : nombre_caissiers > 1 \n"); 
        usage(argv[0]);
    }

    if(m.nb_clients < 1){
        fprintf(stderr,"usage : nombre_clients > 1 \n"); 
        usage(argv[0]);
    }

    // Initialisation de la file de message 

    /* Génération d'une clé unique */
    key = ftok(".",'M');
    if (key == 1){
        perror("Erreur ftok");
        return 1;
    }

    sprintf(str_key, "%d", key);

    /* Création de la file de messages */
    msgid = msgget(key, 0666 | IPC_CREAT);
    if (msgid == -1){
        perror("Erreur msgget");
        return 1;
    }


    for (i=0; i<m.nb_vendeurs;i++){

        m.tab_vendeurs[i] = fork();
            
        if( m.tab_vendeurs[i] < -1){
            fprintf(stderr,"Erreur de création de processus \n");
            exit(EXIT_FAILURE);
        }else if( m.tab_vendeurs[i] == 0 ){
            printf("Vendeur créé : %d \n", getpid());
            
            sprintf(num_creation, "%d", i+1);

            char * args[] = {"./vendeurs", str_nb_vendeurs, str_nb_caissiers, str_nb_clients, num_creation, str_key, NULL}; // Préparation des arguments pour execv

            if( execv("./vendeurs", args) == -1 ) {
                perror("execv vendeurs failed");
                exit(EXIT_FAILURE);
            }

            exit(0);
        }
    }
    

    for (i=0; i<m.nb_caissiers;i++){
        m.tab_caissiers[i] = fork();
            
        if( m.tab_caissiers[i] < -1){
            fprintf(stderr,"Erreur de création de processus \n");  
            exit(EXIT_FAILURE);
        }else if( m.tab_caissiers[i] == 0 ){
            printf("Caissier créé : %d \n", getpid());
            
            sprintf(num_creation, "%d", i);

            char * args[] = {"./caissiers", str_nb_vendeurs, str_nb_caissiers, str_nb_clients, num_creation, str_key, NULL}; // Préparation des arguments pour execv

            if( execv("./caissiers", args) == -1 ) {
                perror("execv caissiers failed");
                exit(EXIT_FAILURE);
            }

            exit(0);
        }
    }

    for (i=0; i<m.nb_clients;i++){

        m.tab_clients[i] = fork();
            
        if( m.tab_clients[i] < -1){
            fprintf(stderr,"Erreur de création de processus \n");  
            exit(EXIT_FAILURE);
        }else if( m.tab_clients[i] == 0 ){
            printf("Client créé : %d \n", getpid());

            sprintf(num_creation, "%d", i);
            
            char * args[] = {"./clients", str_nb_vendeurs, str_nb_caissiers, str_nb_clients, num_creation, str_key, NULL}; // Préparation des arguments pour execv

            if( execv("./clients", args) == -1 ) {
                perror("execv clients failed");
                exit(EXIT_FAILURE);
            }

            exit(0);
        }
    }

    for (j=0; j<m.nb_clients;j++){
        kill(m.tab_clients[j], SIGUSR1);
    }


    /* On attend la terrminaison : */
    fprintf(stderr,"Attente (robuste) de leur terminaison");
    attente = 0;
    while (attente < m.nb_clients){ 

        if ((waitpid(m.tab_clients[attente],NULL,0) == -1) && (errno == ECHILD)){
            attente ++;
        }else{
            //fprintf(stderr,".");
        }
    }
    fprintf(stderr,"\n");



    fprintf(stderr," Attente du kill de %d pour terminer \n", getpid());
    if (sigaction(SIGTERM, &sa, NULL) < 0) {
        perror("Erreur sigaction");
        exit(1);
    }
    while (!sigtermrecu){
        pause();
    }




    // Avertir vendeurs et caissiers qu'ils peuvent terminer proprement

    // Détruire les IPCs

    for (j=0; j<m.nb_vendeurs;j++){
        kill(m.tab_vendeurs[j], SIGUSR1);
    }
    for (j=0; j<m.nb_caissiers;j++){
        kill(m.tab_caissiers[j], SIGUSR1);
    }


    // Destruction file de message
    msgctl(msgid, IPC_RMID, NULL);

    exit(EXIT_SUCCESS);
}