#include "librairie.h"

struct message{
    long mtype;
    char mtext[100]; 
};

int main(){
    struct message msg; 

    key_t cle = ftok("./",'a');  
    int msgid = msgget(cle,0666);  

    msgrcv(msgid,&msg,sizeof(msg.mtext),1,0); 

    fprintf(stdout,"Message reçu: %s \n",msg.mtext); 

    msgctl(msgid,IPC_RMID,NULL);

    exit(EXIT_SUCCESS); 
}