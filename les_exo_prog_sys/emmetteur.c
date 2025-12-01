#include "librairie.h"

struct message{
    long mtype; 
    char mtext[100] ; 
};

int main(){
    struct message msg;
    int msgid; 

    msg.mtype = 1;  
    strcpy(msg.mtext ,"Bonjour"); 
    key_t cle = ftok("./",'a');
    
    if(cle == -1){
        perror("ftok"); 
        exit(EXIT_FAILURE);  
    }

    if( (msgid = msgget(cle,IPC_CREAT | 0666)) == -1){
        perror("msgget");
        exit(EXIT_FAILURE);       
    }
    
    if(msgsnd(  msgid,                      // obtenu par msgget
                 &msg,                      // pointeur sur le message
    sizeof(msg.mtext),                      // la taille de message 
        0) == -1                            // aucun flag 
    ){
        perror("msgsnd");
        exit(EXIT_FAILURE); 
    }

    fprintf(stdout,"Message envoyé\n"); 

    exit(EXIT_SUCCESS); 
}