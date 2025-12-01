CC = gcc
CFLAGS = -Wall -O2 

OBJ= client.o serveur.o

all: serveur client 

serveur: serveur.o
	$(CC) $(CFLAGS) -o serveur serveur.o

client: client.o
	$(CC) $(CFLAGS) -o client client.o

client.o:client.c
	$(CC) $(CFLAGS) -c client.c -o client.o

serveur.o: serveur.c
	$(CC) $(CFLAGS) -c serveur.c -o serveur.o

propre:
	rm -fr *.o serveur client cle.serveur