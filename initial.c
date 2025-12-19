#include "librairies.h"

int sigtermrecu = 0; 

void arret(int sig){
    fprintf(stderr,"signal %d (TERM) reçu (Initial) \n", sig); 
    sigtermrecu++;
}

void initial_sigusr1(int sig){
    
}


void usage(char * appel){
    fprintf(stdout,"Usage : %s <nombre vendeurs> <nombre caissiers> <nombre clients> \n", appel);
    exit(EXIT_FAILURE);
}

void fonc_dest(magasin *shm_ptr,int shmid,int msgid,int sem_id){
    shmdt(shm_ptr);//Détachement  
    shmctl(shmid, IPC_RMID, NULL);//Destruction
    semctl(sem_id, 0, IPC_RMID);//Destruction 
     
    msgctl(msgid, IPC_RMID, NULL);// Destruction file de message
}

void init_struct(magasin* shm_ptr,int nbVend,int nbCaisse){
    int i;
    shm_ptr->nb_rayon_inoccupe = NB_RAYON;
    for (i=0;i<NB_RAYON;i++){
        shm_ptr->tab_rayon_inoccupe[i] = i;
    }

    // Init vendeurs
    for (i = 0; i < nbVend; i++){
        shm_ptr->tab_vendeurs[i].numero = i;
        shm_ptr->tab_vendeurs[i].nb_clients_attente = 0;
        shm_ptr->tab_vendeurs[i].client_actuel = -1; 
        shm_ptr->tab_vendeurs[i].etat = LIBRE; 

        if(i < NB_RAYON){
            shm_ptr->tab_vendeurs[i].rayon_expertise = i;
        } else {
            shm_ptr->tab_vendeurs[i].rayon_expertise = rand() % NB_RAYON;
        }
    }

    
    for (i = 0; i < nbCaisse; i++){
        shm_ptr->tab_caissiers[i].numero = i;  
        shm_ptr->tab_caissiers[i].nb_clients_attente = 0;  
        shm_ptr->tab_caissiers[i].client_actuel = -1; 
        shm_ptr->tab_caissiers[i].etat = LIBRE; 
    }
}

void init_semaph(int sem_id){
    int i; 
    for(i = 0; i < 5 ; i++){
        if( semctl(sem_id,i,SETVAL,1) == -1){
            perror("Erreur semctl SETVAL");
            exit(EXIT_FAILURE);
        }
    }
}

int main(int argc, char* argv[]){
    if (argc != 4){
        usage(argv[0]);
    }
    int i,j; //int attente;
    int fic_log;
    pid_t pid; // Pour le waitpid

    if( ( fic_log = open("log.txt", O_WRONLY | O_CREAT | O_APPEND | O_TRUNC, 0666)) == -1 ){
        fprintf(stdout,"Erreur lors de l'ouverture du fichier\n");
        exit(EXIT_FAILURE); 
    }

    fcntl(fic_log, F_SETFD, 0);// empêche la fermeture automatique

    int nb_vendeurs = atoi(argv[1]);
    int nb_caissiers = atoi(argv[2]);
    int nb_clients = atoi(argv[3]);

    char num_creation[NB_CHAR_EXEC];
    char str_nb_vendeurs[NB_CHAR_EXEC];
    char str_nb_caissiers[NB_CHAR_EXEC];
    char str_nb_clients[NB_CHAR_EXEC];
    char str_fic_log[NB_CHAR_EXEC]; 

    key_t key; char str_key[NB_CHAR_EXEC];

    magasin *shm_ptr;//L'espace mémoire de procecessus 
    int msgid;
    int shmid; // seg.mem.part CLIENT <=> VENDEUR et CLIENT <=> CASIER
    liste_pid l;
    for(i=0;i<nb_clients;i++){l.tab_clients[i] = 0;}
    for(i=0;i<nb_vendeurs;i++){l.tab_vendeurs[i] = 0;}
    for(i=0;i<nb_caissiers;i++){l.tab_caissiers[i] = 0;}
    
    struct sigaction sa_usr1;
    sa_usr1.sa_handler = initial_sigusr1;
    sigemptyset(&sa_usr1.sa_mask);
    sa_usr1.sa_flags = 0;

    if (sigaction(SIGUSR1, &sa_usr1, NULL) < 0) {
        perror("sigaction SIGUSR1");
        exit(EXIT_FAILURE);
    }


    struct sigaction sa;

    sa.sa_handler = arret;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;


    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGUSR1);
    sigprocmask(SIG_BLOCK, &mask, NULL);

    // Initialisation des rayons inoccupés pour s'assurer qu'ils sont tous occupés
    sprintf(str_nb_vendeurs,  "%d", nb_vendeurs);  // Conversion des entiers en chaînes de caractères
    sprintf(str_nb_caissiers, "%d", nb_caissiers);
    sprintf(str_nb_clients,   "%d", nb_clients );
    sprintf(str_fic_log,      "%d", fic_log); 

    // Créer processus vendeur, caissier et clients

    if(nb_vendeurs < NB_RAYON){
        fprintf(stderr,"nombre_vendeurs > nombre de rayon : %d \n",NB_RAYON);
        dprintf(fic_log,"nombre_vendeurs > nombre de rayon : %d \n",NB_RAYON);
        
        usage(argv[0]);
    }

    if(nb_caissiers < 1){
        fprintf(stderr,"usage : nombre_caissiers > 1 \n"); 
        dprintf(fic_log,"usage : nombre_caissiers > 1 \n"); 
        
        usage(argv[0]);
    }

    if(nb_clients < 2){
        fprintf(stderr,"usage : nombre_clients > 1 \n"); 
        dprintf(fic_log,"usage : nombre_clients > 1 \n"); 
        
        usage(argv[0]);
    }

    // Initialisation de la file de message 
    /* Génération d'une clé unique */
    key = ftok(".",'M');
    if (key == -1){
        perror("Erreur ftok : Ligne 81");
        dprintf(fic_log,"Erreur ftok : Ligne 81");
        return 1;
    }

    sprintf(str_key, "%d", key);

    /* Création de la file de messages */
    msgid = msgget(key, 0666 | IPC_CREAT);
    if (msgid == -1){
        perror("Erreur msgget : Ligne 90");
        dprintf(fic_log,"Erreur msgget : Ligne 90");

        return 1;
    }

    //Creation de seg.mémoire.partagé
    fprintf(stderr,"sizeof(magasin) = %lu\n", sizeof(magasin));
    shmid = shmget(key,sizeof(magasin),IPC_CREAT | 0666);

    if (shmid == -1){
        perror("erreur shmget : Ligne 98 ");
        dprintf(fic_log,"erreur shmget : Ligne 98");
        
        exit(EXIT_FAILURE);
    }

    // Attacher le segment au processus 
    shm_ptr = shmat(shmid,NULL,0);

    if (shm_ptr == (magasin*)-1){
        perror("erreur shmat : Ligne 106");
        dprintf(fic_log,"erreur shmat : Ligne 106");
        exit(-1);
    }

    memset(shm_ptr, 0, sizeof(magasin));

    shm_ptr->nb_vendeurs = atoi(argv[1]);
    shm_ptr->nb_caissiers = atoi(argv[2]);
    shm_ptr->nb_clients = atoi(argv[3]);
    shm_ptr->SimulationActive = 1;
    
    //Creation l'ensemble de semaphore (il y en a 5)
    int sem_id = semget(key, 5, IPC_CREAT | 0666);
    if(sem_id == -1){
        perror("erreur semget 136"); 
        dprintf(fic_log,"erreur semget 136"); 
        exit(EXIT_FAILURE);
    }

    init_semaph(sem_id);

    //============================================== UTILISATION =================================== // 
    init_struct(shm_ptr,nb_vendeurs,nb_caissiers);

    for (i=0; i<shm_ptr->nb_vendeurs;i++){
        l.tab_vendeurs[i] = fork();

        if( l.tab_vendeurs[i] == -1){
            fprintf(stderr,"Erreur de création de processus \n");
            exit(EXIT_FAILURE);
        }else if( l.tab_vendeurs[i] == 0 ){
            printf("Vendeur créé : %d \n", getpid());
            dprintf(fic_log,"Vendeur créé : %d \n", getpid());
            
            sprintf(num_creation, "%d", i);

            char * args[] = {"./vendeurs", str_nb_vendeurs, str_nb_caissiers, str_nb_clients, num_creation, str_key,str_fic_log,NULL}; // Préparation des arguments pour execv

            sigprocmask(SIG_UNBLOCK, &mask, NULL);

            if( execv("./vendeurs", args) == -1 ) {
                perror("execv vendeurs failed");
                exit(EXIT_FAILURE);
            }

            exit(0);
        }
    }
    

    for (i=0; i< shm_ptr->nb_caissiers;i++){

        l.tab_caissiers[i] = fork();

        if( l.tab_caissiers[i] < -1){
            fprintf(stderr,"Erreur de création de processus \n");  
            exit(EXIT_FAILURE);
        }else if( l.tab_caissiers[i] == 0 ){
            fprintf(stderr,"Caissier créé : %d \n", getpid());
            dprintf(fic_log,"Caissier créé : %d \n", getpid());
            
            sprintf(num_creation, "%d", i);

            char * args[] = {"./caissiers", str_nb_vendeurs, str_nb_caissiers, str_nb_clients, num_creation, str_key,str_fic_log,NULL}; // Préparation des arguments pour execv

            sigprocmask(SIG_UNBLOCK, &mask, NULL);

            if( execv("./caissiers", args) == -1 ) {
                perror("execv caissiers failed");
                exit(EXIT_FAILURE);
            }

            exit(0);
        }
    }

    for (i=0; i< shm_ptr->nb_clients;i++){

        l.tab_clients[i] = fork();

        if( l.tab_clients[i] == -1){
            fprintf(stderr,"Erreur de création de processus \n");  
            dprintf(fic_log,"Erreur de création de processus \n");  
            
            exit(EXIT_FAILURE);
        }else if( l.tab_clients[i] == 0 ){
            fprintf(stderr,"Client créé : %d \n", getpid());
            dprintf(fic_log,"Client créé : %d \n", getpid());

            sprintf(num_creation, "%d", i);
            
            char * args[] = {"./clients", str_nb_vendeurs, str_nb_caissiers, str_nb_clients, num_creation, str_key,str_fic_log, NULL}; // Préparation des arguments pour execv

            sigprocmask(SIG_UNBLOCK, &mask, NULL);

            fprintf(stderr,"TEST ");
            if( execv("./clients", args) == -1 ) {
                perror("execv clients failed");
                exit(EXIT_FAILURE);
            }

            exit(0);
        }
    }

    sigprocmask(SIG_UNBLOCK, &mask, NULL);

    sleep(1);

    // Debuter tous
    for (j=0; j<shm_ptr->nb_clients;j++){
        kill(l.tab_clients[j], SIGUSR1);
    }
    for (j=0; j<shm_ptr->nb_vendeurs;j++){
        kill(l.tab_vendeurs[j], SIGUSR1);
    }
    for (j=0; j<shm_ptr->nb_caissiers;j++){
        kill(l.tab_caissiers[j], SIGUSR1);
    }

    sleep(2);

    for (i = 0; i < shm_ptr->nb_clients; i++) {
        fprintf(stderr, "attente du client[%d] pid=%d\n",i, l.tab_clients[i]);
    }

    /* On attend la terrminaison : */
    fprintf(stderr,"Attente (robuste) de leur terminaison \n");
    dprintf(fic_log,"Attente (robuste) de leur terminaison \n");
    for (i = 0; i < shm_ptr->nb_clients; i++) {

        pid = l.tab_clients[i];

        if (pid <= 0) {
            fprintf(stderr, "PID client[%d] invalide (%d)\n", i, pid);
            continue;
        }

        waitpid(pid, NULL, 0);
    }
    fprintf(stderr,"\n");

    sleep(2);

    fprintf(stderr," Attente de la commande : kill %d \npour terminer \n\n", getpid());
    if (sigaction(SIGTERM, &sa, NULL) < 0) {
        perror("Erreur sigaction");
        exit(1);
    }
    while (!sigtermrecu){
        pause();
    }

    // Avertir vendeurs et caissiers qu'ils peuvent terminer proprement

    // Détruire les IPCs
    for (j=0; j<shm_ptr->nb_vendeurs;j++){
        kill(l.tab_vendeurs[j], SIGUSR2);
    }
    for (j=0; j<shm_ptr->nb_caissiers;j++){
        kill(l.tab_caissiers[j], SIGUSR2);
    }

    // attendre vendeurs / caissiers
    for (j=0; j<shm_ptr->nb_vendeurs; j++)
        waitpid(l.tab_vendeurs[j], NULL, 0);

    for (j=0; j<shm_ptr->nb_caissiers; j++)
        waitpid(l.tab_caissiers[j], NULL, 0);

    fonc_dest(shm_ptr,shmid,msgid,sem_id); 
    close(fic_log);
    exit(EXIT_SUCCESS);
}

