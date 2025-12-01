CFLAGS = -Wall -O2

TARGET1 = clientUDP serveurUDP
TARGET2 = clientTCP serveurTCP 

make: $(TARGET2)

clientUDP: clientUDP.c
	gcc $(CFLAGS) clientUDP.c -o clientUDP

serveurUDP:serveurUDP.c
	gcc $(CFLAGS) serveurUDP.c -o serveurUDP

clean:
	rm -fr *.o clientUDP serveurUDP clientTCP serveurTCP $(TARGET1) $(TARGET2)