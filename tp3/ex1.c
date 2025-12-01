#include "../libraries.h"

void usage(){
    fprintf(stdout,"Usage : exo1 <nombre> [<faces> [<limite>]] \n");
    fprintf(stdout,"\t faces par defaut : 12 \n");
    fprintf(stdout,"\t par defaut : 10 \n");
}

int main(int argc,char * argv[]){
    int n = atoi(argv[1]); 
    int i = 1;    
    
    if( argc < 0 ){
        usage(); 
        exit(EXIT_FAILURE);
    }


    

    exit(EXIT_SUCCESS); 
}