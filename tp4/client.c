 /*
 * Client : il recupere (l'id d'une) une file de messages, envoie une requete
 * au serveur, attend la reponse, l'ecrit sur stdout
 */
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <signal.h>
#include <string.h>

#include "types.h"

void usage(char *s){
    fprintf(stderr,"Usage : %s gauche op droite\n\tgauche, droite : entiers\n\top = +, -, *, /, \%% \n",s);
    exit(-1);
}

char atoc(const char *str) {
    return str[0];
}


int main (int argc, char *argv[]){
    key_t cle; /* cle de la file     */
    int file_mess; /* ID de la file    */
    FILE *fich_cle;
    int g,d,op; 
    requete_t requete;
    reponse_t reponse;

    if(argc < 4 ){
        usage(argv[0]);
    }

    /* Recuperation des arguments                   */
    g = atoi(argv[1]); op = atoc(argv[2]); d = atoi(argv[3]);  
    requete.g = g; 
    requete.op = op;
    requete.d = d; 
    requete.expediteur = getpid(); 
    
    /* Creation de la cle :                         */
    /* 1 - On teste si le fichier cle existe dans le repertoire courant : */

    fich_cle = fopen(FICHIER_CLE,"r");
    if (fich_cle==NULL){
        fprintf(stderr,"Lancement client impossible\n");
        exit(-1);
    }
    
    cle = ftok(FICHIER_CLE,'a');
    if (cle==-1){
        fprintf(stderr,"Pb creation cle\n");
        exit(-1);
    }

    /* Recuperation file de message :               */
    if((file_mess = msgget(cle,0666)) == -1){
        fprintf(stderr,"Récupération file de message impossible\n");
        fprintf(stderr, "Erreur (errno): %d - %s\n", errno, strerror(errno));
        exit(-1);
    } 

    /* creation de la requete :                     */
     requete.type = 1; 

    /* envoi de la requete :                        */
    if( (msgsnd(file_mess,&requete,sizeof(requete),IPC_NOWAIT)) == -1){
        fprintf(stderr,"Envoie de message impossible\n");
        fprintf(stderr,"Erreur (errno): %d - %s\n", errno, strerror(errno));
        exit(-1);
    }else{
        fprintf(stdout,"Le client %d envoie %d %c %d \n",
            requete.expediteur,
            requete.g,
            requete.op,
            requete.d); 
    }


    /* attente de la reponse :                      */
    if( (msgrcv(file_mess,&reponse,sizeof(reponse),reponse.type,0)) == -1){
        fprintf(stderr,"Reception de reponse impossible\n");
        fprintf(stderr,"Erreur (errno): %d - %s\n", errno, strerror(errno));
        exit(-1);
    }


    /* affichage du resultat sur stdout :           */
    fprintf(stdout,"Le client %d recoit le resultat %d \n",getpid(),reponse.resu); 
    
    
    exit(0);
}
