CFLAGS = -Wall -O2

TARGET = prog_init producteurs consomateurs

make: $(TARGET)

prog_init: prog_init.c 
	gcc $(CFLAGS) prog_init.c -o prog_init

producteurs:producteurs.c 
	gcc $(CFLAGS) producteurs.c -o producteurs

consomateurs:consomateurs.c 
	gcc $(CFALGS) consomateurs.c -o consomateurs
	
test_file_circulaire: TAD_file_circulaire.c
		gcc $(CFLAGS)  TAD_file_circulaire.c  -o test_file_circulaire

clean:
	rm -fr *.o test_file_circulaire prog_init $(TARGET)