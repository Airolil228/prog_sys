#include <stdio.h>  
#include <stdlib.h>
#include <errno.h>
#include <unistd.h> 


int main(int argc,char *argv[]){

    if(argc < 2){
        printf("Usage: %s arg1 arg2 \n",argv[0]);
        exit(EXIT_FAILURE);
    }
    char *  newargv[] ={ argv[1], argv[2], NULL }; 

    if (execve(argv[1], newargv , NULL  )== -1) {
        perror("Erreur d'execv");
        exit(EXIT_FAILURE);
    }

    switch(errno){
        case ENOENT: 
            printf("Le fichier n'existe pas\n");
            break;
        case    EACCES:
            printf("Le fichier n'est pas exécutable\n");    
            break;
         case E2BIG:
            printf("Le tableau d'arguments est trop grand\n");
            break;
        case ELOOP:
            printf("Trop de liens symboliques ont été rencontrés lors du suivi des liens symboliques\n");
            break;
        case EFAULT:
            printf("Un des arguments pointe sur une zone interdite \n");
            break;

    }

     


    exit(EXIT_SUCCESS);
}