
all: acharKMenores

acharKMenores: acharKMenores.c 
	g++ -O3 -pthread acharKMenores.c -o acharKMenores

.PHONY: clean
clean: 
	-rm -f *.o 
purge: clean
	-rm acharKMenores

