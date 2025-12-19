CC = gcc
CFLAGS = -Wall -O2

TARGET = initial vendeurs caissiers clients monitoring

make: $(TARGET)

initial : initial.c
	$(CC) $(CFLAGS) initial.c -o initial

vendeurs : vendeurs.c
	$(CC) $(CFLAGS) vendeurs.c -o vendeurs

caissiers : caissiers.c
	$(CC) $(CFLAGS) caissiers.c -o caissiers

clients : clients.c
	$(CC) $(CFLAGS) clients.c -o clients

monitoring: monitoring.c
	$(CC) $(CFLAGS) monitoring.c -o monitoring

clean:
	rm -fr *.o $(TARGET) 