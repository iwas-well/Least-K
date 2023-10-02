
all: acharKMenores

acharKMenores: acharKMenores.c 
	g++ acharKMenores.c -o acharKMenores

.PHONY: clean
clean: 
	-rm -f *.o 
purge: clean
	-rm acharKMenores

