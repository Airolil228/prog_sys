 /*
 * Serveur : il crée une file de messages, attend des messages 
 * des clients, y repond.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <signal.h>

#include "types.h"

int file_mess; /* ID de la file    */

void arret(int s){
    /*---------------------------------
     !                                ! 
     !                                ! 
     !         A completer !          ! 
     !                                ! 
     !                                ! 
     ---------------------------------*/
}

void mon_sigaction(int signal, void (*f)(int)){
    struct sigaction action;
 
    action.sa_handler = f;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;
    sigaction(signal,&action,NULL);
}


int main (int argc, char *argv[]){
    key_t cle; /* cle de la file     */
    FILE *fich_cle;
    requete_t requete;
    reponse_t reponse;
    int nb_lus,flag;
    

     /* Creation de la cle :                                */
    /* 1 - On teste si le fichier cle existe dans le repertoire courant : */
    fich_cle = fopen(FICHIER_CLE,"r");
    if (fich_cle==NULL){
        if (errno==ENOENT){
            /* on le cree */
            fich_cle=fopen(FICHIER_CLE,"w");
            if (fich_cle==NULL){
                fprintf(stderr,"Lancement serveur impossible\n");
                exit(-1);
            }
        }
        else {
            fprintf(stderr,"Lancement serveur impossible\n");
            exit(-1);
        }
    }

    cle = ftok(FICHIER_CLE,'a');
    if (cle==-1){
        fprintf(stderr,"Pb creation cle\n");
        exit(-1);
    }
    
    flag = IPC_CREAT|IPC_EXCL|0666;
    
    /* Creation file de message :                          */
    if( (nb_lus = msgget(cle, flag)) == -1){
        fprintf(stderr,"création le file de message 1impossible \n");
        fprintf(stderr, "Erreur (errno): %d - %s\n", errno, strerror(errno));
        exit(EXIT_FAILURE); 
    }
    
    mon_sigaction(SIGUSR1,arret);

    for(;;){ /* Indefiniment :*/
        /* serveur attend des requetes                     */
        if( msgrcv(nb_lus,&requete,sizeof(requete),requete.type,0) == -1 ){
            fprintf(stderr,"création le file de message impossible \n");
            fprintf(stderr, "Erreur (errno): %d - %s\n", errno, strerror(errno));
            exit(EXIT_FAILURE);
        }else{
            fprintf(stdout,"(Serveur) requete recue de %d \n",requete.expediteur); 
            fprintf(stdout,"op=%c g=%d d=%d \n",requete.op,requete.g,requete.d);
        }

        /* traitement de la requete :                      */
        switch(requete.op){
            case '+': 
                reponse.resu = requete.g+requete.d; 
            break;  
            case '-': 
                reponse.resu = requete.g-requete.d; 
            break;
            case '*': 
                reponse.resu = requete.g*requete.d; 
            break; 
            case '/':
            if(requete.d == 0){
                fprintf(stderr,"On divise pas par 0, salle con !!!\n"); 
                exit(-1); 
            }
                reponse.resu = requete.g/requete.d; 
            break;
        }

        reponse.type = 1;
        /* envoi de la reponse :                           */
        if( (msgsnd(nb_lus,&reponse,sizeof(reponse),IPC_NOWAIT)) == -1){
            fprintf(stderr,"Envoie de reponse  impossible\n");
            fprintf(stderr,"Erreur (errno): %d - %s\n", errno, strerror(errno));
            exit(-1);        
        }
    }   
    /* pour gcc */
    exit(0);
}


