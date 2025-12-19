CC = gcc
CFLAGS = -Wall -O2

TARGET = initial vendeurs caissiers clients

make: $(TARGET)

initial : initial.c
	$(CC) $(CFLAGS) initial.c -o initial

vendeurs : vendeurs.c
	$(CC) $(CFLAGS) vendeurs.c -o vendeurs

caissiers : caissiers.c
	$(CC) $(CFLAGS) caissiers.c -o caissiers

clients : clients.c
	$(CC) $(CFLAGS) clients.c -o clients

clean:
	rm -fr *.o $(TARGET) 