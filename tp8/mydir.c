/* 
 *   ls du rep courant
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
#include <time.h>

/*
 *
 * Affiche le type du fichier :
 *     - fichier regulier     : fic
 *     - repertoire           : rep
 *     - tube                 : tub
 *     - fichier ES caractere : car 
 *     - fichier ES bloc      : blc 
 *     - lien symbolique      : sym
 *     - socket               : sck 
 *
 */
void afficher_type(mode_t mode){
   switch (mode & S_IFMT) {
    case S_IFBLK:  printf("blc ");                       break;
    case S_IFCHR:  printf("car ");                       break;
    case S_IFDIR:  printf("rep ");                       break;
    case S_IFIFO:  printf("tub ");                       break;
    case S_IFLNK:  printf("sym ");                       break;
    case S_IFREG:  printf("fic ");                       break;
    case S_IFSOCK: printf("sck ");                       break;
    default:       printf("inconnu ?");                 break;
    }
}

/*
 * Affiche tous les droits du fichier :
 * - droits speciaux        : (SGD)
 * - droits du proprietaire : (LEX)
 * - droits du proprietaire : (LEX)
 * - droits du proprietaire : (LEX)
 *
 *  (. si droit inexistant)
 */
void afficher_droits(mode_t mode)
{
    /* --- Droits spéciaux (SGD) --- */
    printf("(%c", (mode & S_ISUID) ? 'S' : '.');  // Set-UID
    printf("%c", (mode & S_ISGID) ? 'G' : '.');  // Set-GID
    printf("%c)", (mode & S_ISVTX) ? 'D' : '.');  // Sticky bit
    printf(" ");

    /* --- Droits du propriétaire (LEX) --- */
    printf("(%c", (mode & S_IRUSR) ? 'L' : '.');  // Lecture
    printf("%c", (mode & S_IWUSR) ? 'E' : '.');  // Écriture
    printf("%c)", (mode & S_IXUSR) ? 'X' : '.');  // Exécution
    printf(" ");

    /* --- Droits du groupe (LEX) --- */
    printf("(%c", (mode & S_IRGRP) ? 'L' : '.');
    printf("%c", (mode & S_IWGRP) ? 'E' : '.');
    printf("%c)", (mode & S_IXGRP) ? 'X' : '.');
    printf(" ");

    /* --- Droits des autres (LEX) --- */
    printf("(%c", (mode & S_IROTH) ? 'L' : '.');
    printf("%c", (mode & S_IWOTH) ? 'E' : '.');
    printf("%c)", (mode & S_IXOTH) ? 'X' : '.');

}

int main(int argc, char *argv[]){

    //*   Declaration des variables       *
    DIR * rep_courant;
    struct dirent * lire_rep; 
    struct stat sb;//stat buffer 

    int du_boulot = 1; 
    //   Ouverture du rep courant        
    if( (rep_courant = opendir(".")) ==  NULL){
        perror("opendir erreur"); 
        exit(EXIT_FAILURE);
    }
    
    while (du_boulot){
	
	//*   Lecture entree suivante,        *
	//*   sortie si fin de repertoire     *

	if( (lire_rep = readdir(rep_courant)) == NULL ){
        du_boulot = 0; 
        break; 
    }

	//*   Acquisition de l'inode          *
	if( stat(lire_rep->d_name,&sb)){
        perror("stat erreur");
        exit(EXIT_FAILURE); 
    }

	afficher_type(sb.st_mode);
	fprintf(stderr," ");

	afficher_droits(sb.st_mode);
	fprintf(stderr," ");

	
	// *   Afficher la taille de l'entree  *
	fprintf(stdout," %ld ",(long) sb.st_size);

	// *   Afficher le nom de l'entree     *
    fprintf(stdout,"   %s \n",lire_rep->d_name);

    }

    
    // *   Fermeture du rep courant        *
    closedir(rep_courant);
    printf("\n");
    /* Fini */
    exit(0);
}