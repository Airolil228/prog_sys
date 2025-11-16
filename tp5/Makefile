CC = gcc
CFLAGS = -Wall -O2 

TARGET = pere fils 

make: $(TARGET)

pere: version1.o
		$(CC) $(CFLAGS) -o pere version1.o
fils: fils.o
		$(CC) $(CFLAGS) -o fils fils.o

fils.o:fils.c
	$(CC) $(CFLAGS) -c fils.c -o fils.o

version.o:version.c
	$(CC) $(CFLAGS) -c version.c -o version.o

propre:
	rm -fr *.o serveur client cle.serveur $(TARGET)