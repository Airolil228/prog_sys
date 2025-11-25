CC = gcc
CFLAGS = -Wall -O2 

TARGET1 = pere1 fils1 
TARGET2 = pere2 fils2

make: $(TARGET2)

pere2: version2.o
		$(CC) $(CFLAGS) -o pere2 version2.o

fils2: fils2.o
		$(CC) $(CFLAGS) -o fils2 fils2.o

version2.o:version2.c
	$(CC) $(CFLAGS) -c version2.c -o version2.o

fils2.o:fils2.c
	$(CC) $(CFLAGS) -c fils2.c -o fils2.o

pere1: version.o
		$(CC) $(CFLAGS) -o pere1 version.o

fils1: fils.o
		$(CC) $(CFLAGS) -o fils1 fils.o

fils.o:fils.c
	$(CC) $(CFLAGS) -c fils.c -o fils.o

version.o:version1.c
	$(CC) $(CFLAGS) -c version1.c -o version.o

propre:
	rm -fr *.o serveur client cle.serveur $(TARGET1) $(TARGET2)