#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <time.h> 
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <time.h>

#define NB_CHAR_EXEC 16
#define NB_RAYON 10

#define MONTANT_MAX 100
#define MONTANT_MIN 10

#define PROBA_ACHAT 60
#define TAILLE 1026

int nb_rayon_inoccupe;
int tab_rayon_inoccupe[NB_RAYON];

typedef enum{
    LIBRE = 0,
    OCCUPE = 1
}Etat;

typedef struct{
    pid_t pid;
    int numero;
    int rayon_expertise;
    int nb_clients_attente;
    int client_actuel;
    Etat etat;
}InfoVend;

typedef struct{
    pid_t pid;
    int numero;
    int nb_clients_attente;
    int client_actuel;
    Etat etat;
}InfoCassier; 

typedef struct{
    pid_t pid;
    int montant;
    int valide;  
}InfoClient;

typedef struct { //
    int tab_rayon[NB_RAYON];

    int nb_vendeurs;
    InfoVend tab_vendeurs[100];
    
    int nb_caissiers;
    InfoCassier tab_caissiers[100];
    
    int nb_clients;

    InfoClient tab_clients[100];

    int SimulationActive; 
}magasin;

struct msgbuf {
    long mtype ;
    int client_id;
    int rayon;
    int vendeur_reco;    // vendeur recommandé si pas bon rayon
    int decision;        // 0 = refuse, 1 = accepte
    int montant;
    int num_vendeur;
    int num_caissier; 
    int type_message;    // REPONSE/DEMANDE 
};

struct sigaction action;