#include "librairies.h"

int sigtermrecu = 0; 
magasin m;

void arret(int sig){
    printf("signal %d (TERM) reçu (Initial) \n", sig); 
    sigtermrecu++;
}

int sigusr1recu = 0; 

void debut(int sig){
    printf("Le signal %d commence (dans Initial.c) \n", sig); 
    sigusr1recu++;
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

void init_struct(int nbVend,int nbCaisse){
    // Init vendeurs
    for(int i = 0; i < nbVend; i++){
        m.tab_vendeurs[i].numero = i;
        m.tab_vendeurs[i].nb_clients_attente = 0;
        m.tab_vendeurs[i].client_actuel = -1; 
        m.tab_vendeurs[i].etat = LIBRE; 
        m.tab_vendeurs[i].pid = 0; // Initialisation du pid (recommandé)

        if(i < NB_RAYON){
            m.tab_vendeurs[i].rayon_expertise = i;
        } else {
            m.tab_vendeurs[i].rayon_expertise = rand() % NB_RAYON;
        }
    }

    
    for(int i = 0; i < nbCaisse; i++){
        m.tab_caissiers[i]. numero = i;  
        m.tab_caissiers[i].nb_clients_attente = 0;  
        m.tab_caissiers[i].client_actuel = -1; 
        m.tab_caissiers[i].etat = LIBRE; 
        m.tab_caissiers[i].pid = 0; 
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
    int i,j; int attente;
    int fic_log;

    if( ( fic_log = open("log.txt", O_WRONLY | O_CREAT | O_APPEND | O_TRUNC, 0666)) == -1 ){
        fprintf(stdout,"Erreur lors de l'ouverture du fichier\n");
        exit(EXIT_FAILURE); 
    }

    fcntl(fic_log, F_SETFD, 0);// empêche la fermeture automatique

    m.nb_vendeurs = atoi(argv[1]);
    m.nb_caissiers = atoi(argv[2]);
    m.nb_clients = atoi(argv[3])+1;

    char num_creation[NB_CHAR_EXEC];
    char str_nb_vendeurs[NB_CHAR_EXEC];
    char str_nb_caissiers[NB_CHAR_EXEC];
    char str_nb_clients[NB_CHAR_EXEC];
    char str_fic_log[NB_CHAR_EXEC]; 

    key_t key; char str_key[NB_CHAR_EXEC];

    magasin *shm_ptr;//L'espace mémoire de procecessus 
    int msgid;
    int shmid; // seg.mem.part CLIENT <=> VENDEUR et CLIENT <=> CASIER
    
    struct sigaction sa;

    sa.sa_handler = arret;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    struct sigaction sa2;

    sa2.sa_handler = debut;
    sigemptyset(&sa2.sa_mask);
    sa2.sa_flags = 0;

    // Initialisation des rayons inoccupés pour s'assurer qu'ils sont tous occupés
    nb_rayon_inoccupe = NB_RAYON;
    for (i=0;i<NB_RAYON;i++){
        tab_rayon_inoccupe[i] = i;
    }

    sprintf(str_nb_vendeurs,  "%d", m.nb_vendeurs);  // Conversion des entiers en chaînes de caractères
    sprintf(str_nb_caissiers, "%d", m.nb_caissiers);
    sprintf(str_nb_clients,   "%d", m.nb_clients );
    sprintf(str_fic_log,      "%d", fic_log); 

    // Créer processus vendeur, caissier et clients

    if(m.nb_vendeurs < NB_RAYON){
        fprintf(stderr,"nombre_vendeurs > nombre de rayon : %d \n",NB_RAYON);
        dprintf(stderr,"nombre_vendeurs > nombre de rayon : %d \n",NB_RAYON);
        
        usage(argv[0]);
    }

    if(m.nb_caissiers < 1){
        fprintf(stderr,"usage : nombre_caissiers > 1 \n"); 
        dprintf(fic_log,"usage : nombre_caissiers > 1 \n"); 
        
        usage(argv[0]);
    }

    if(m.nb_clients < 1){
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
    printf("sizeof(magasin) = %lu\n", sizeof(magasin));
    //fprintf(fic_log,"sizeof(magasin) = %lu\n", sizeof(magasin));
    
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

    shm_ptr->nb_vendeurs = m.nb_vendeurs;
    shm_ptr->nb_caissiers = m.nb_caissiers;
    shm_ptr->nb_clients = m.nb_clients;
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
    init_struct(m.nb_vendeurs,m.nb_caissiers);

    for (i=0; i<m.nb_vendeurs;i++){
        m.tab_vendeurs[i].pid = fork();

        if( m.tab_vendeurs[i].pid == -1){
            fprintf(stderr,"Erreur de création de processus \n");
            exit(EXIT_FAILURE);
        }else if( m.tab_vendeurs[i].pid == 0 ){
            printf("Vendeur créé : %d \n", getpid());
            dprintf(fic_log,"Vendeur créé : %d \n", getpid());
            
            sprintf(num_creation, "%d", i+1);

            char * args[] = {"./vendeurs", str_nb_vendeurs, str_nb_caissiers, str_nb_clients, num_creation, str_key,str_fic_log,NULL}; // Préparation des arguments pour execv

            if (sigaction(SIGUSR1, &sa2, NULL) < 0) {
                perror("Erreur sigaction");
                exit(1);
            }
            while (!sigusr1recu){
                pause();
            }

            if( execv("./vendeurs", args) == -1 ) {
                perror("execv vendeurs failed");
                exit(EXIT_FAILURE);
            }

            exit(0);
        }
    }
    

    for (i=0; i<m.nb_caissiers;i++){
        m.tab_caissiers[i].pid = fork();

        if( m.tab_caissiers[i].pid < -1){
            fprintf(stderr,"Erreur de création de processus \n");  
            exit(EXIT_FAILURE);
        }else if( m.tab_caissiers[i].pid == 0 ){
            printf("Caissier créé : %d \n", getpid());
            dprintf(fic_log,"Caissier créé : %d \n", getpid());

            sprintf(num_creation, "%d", i);

            char * args[] = {"./caissiers", str_nb_vendeurs, str_nb_caissiers, str_nb_clients, num_creation, str_key,str_fic_log,NULL}; // Préparation des arguments pour execv

            if (sigaction(SIGUSR1, &sa2, NULL) < 0) {
                perror("Erreur sigaction");
                exit(1);
            }
            while (!sigusr1recu){
                pause();
            }

            if( execv("./caissiers", args) == -1 ) {
                perror("execv caissiers failed");
                exit(EXIT_FAILURE);
            }

            exit(0);
        }
    }

    for (i=0; i<m.nb_clients;i++){

        m.tab_clients[i].pid = fork();

        if( m.tab_clients[i].pid == -1){
            fprintf(stderr,"Erreur de création de processus \n");  
            dprintf(fic_log,"Erreur de création de processus \n");  
            
            exit(EXIT_FAILURE);
        }else if( m.tab_clients[i].pid == 0 ){
            printf("Client créé : %d \n", getpid());
            dprintf(fic_log,"Client créé : %d \n", getpid());

            sprintf(num_creation, "%d", i);
            
            char * args[] = {"./clients", str_nb_vendeurs, str_nb_caissiers, str_nb_clients, num_creation, str_key,str_fic_log, NULL}; // Préparation des arguments pour execv

            if (sigaction(SIGUSR1, &sa2, NULL) < 0) {
                perror("Erreur sigaction");
                exit(1);
            }
            while (!sigusr1recu){
                pause();
            }
            if( execv("./clients", args) == -1 ) {
                perror("execv clients failed");
                exit(EXIT_FAILURE);
            }

            exit(0);
        }
    }

    // Debuter tous
    for (j=0; j<m.nb_clients;j++){
        kill(m.tab_clients[j].pid, SIGUSR1);
    }
    for (j=0; j<m.nb_vendeurs;j++){
        kill(m.tab_vendeurs[j].pid, SIGUSR1);
    }
    for (j=0; j<m.nb_caissiers;j++){
        kill(m.tab_caissiers[j].pid, SIGUSR1);
    }


    /* On attend la terrminaison : */
    fprintf(stderr,"Attente (robuste) de leur terminaison \n");
    dprintf(fic_log,"Attente (robuste) de leur terminaison \n");
    
    attente = 0;
    while (attente < m.nb_clients){ 

        if ((waitpid(m.tab_clients[attente].pid,NULL,0) == -1) && (errno == ECHILD)){
            attente ++;
        }else{
            //fprintf(stderr,".");
        }
    }
    fprintf(stderr,"\n");



    fprintf(stderr," Attente de la commande : kill %d \npour terminer \n", getpid());
    if (sigaction(SIGTERM, &sa, NULL) < 0) {
        perror("Erreur sigaction");
        exit(1);
    }
    while (!sigtermrecu){
        pause();
    }

    // Avertir vendeurs et caissiers qu'ils peuvent terminer proprement

    // Détruire les IPCs
    for (j=0; j<m.nb_clients;j++){
        kill(m.tab_clients[j].pid, SIGUSR2);
    }

    for (j=0; j<m.nb_vendeurs;j++){
        kill(m.tab_vendeurs[j].pid, SIGUSR2);
    }

    for (j=0; j<m.nb_caissiers;j++){
        kill(m.tab_caissiers[j].pid, SIGUSR2);
    }

    fonc_dest(shm_ptr,shmid,msgid,sem_id); 
    close(fic_log);
    exit(EXIT_SUCCESS);
}

