CC = gcc
CFLAGS = -Wall -Wextra -O2

dialog: recepteur emmetteur
	$(CC) $(CFLAGS) -o dialog recepteur emmetteur

recepteur: recepteur.o
	$(CC) $(CFLAGS) -o recepteur recepteur.o

emmetteur: emmetteur.o
	$(CC) $(CFLAGS) -o emmetteur emmetteur.o

clean:
	rm -f dialog recepteur emmetteur *.o