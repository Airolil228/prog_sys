all: lancer n_processus attente_terminaison 

attente_terminaison: ex4.o 
	gcc -o attente_terminaison ex4.o

lancer: ex2.o 
	gcc -o lancer ex2.o 

n_processus: ex3.o 
	gcc -o n_processus ex3.o

ex4.o: 
	gcc -c ex4.c

ex2.o: 
	gcc -c ex2.c

ex3.o:
	gcc -c ex3.c

clean: 
	rm -f *.o lancer n_processus attente_terminaison