all : np ns1
.PHONY : all

np : nodoPrimario.c
	gcc -o np nodoPrimario.c

ns1 : nodoSecundario1.c
	gcc -o ns1 nodoSecundario1.c -pthread

 clean :
	$(RM) ns1 np ordenado_testing
