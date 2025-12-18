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

#define NB_CHAR_EXEC 16
#define NB_RAYON 10

int nb_rayon_inoccupe;
int tab_rayon_inoccupe[NB_RAYON];

typedef struct {
    int tab_rayon[NB_RAYON];
    int nb_vendeurs;
    pid_t tab_vendeurs[100];
    int nb_caissiers;
    pid_t tab_caissiers[100];
    int nb_clients;
    pid_t tab_clients[100];
}magasin;

struct msgbuf {
    long mtype ;
    char mtext [100];
}

struct sigaction action;