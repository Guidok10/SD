#include "generala.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

CLIENT *clnt;
int  *result_1;
char *anotarse_1_arg;
int  *result_2;
char *empezar_1_arg;
int  *result_3;
char *jugadores_1_arg;
int  *result_4;
int  permiso_1_arg;
struct jugada  *result_5;
char *jugadaanterior_1_arg;
char * *result_6;
char *resultadosparciales_1_arg;
struct dados  *result_7;
char *jugar_1_arg;
int  *result_8;
struct tabla  anotar_1_arg;
int  *result_9;
struct tabla  tachar_1_arg;
int  *result_10;
int  salir_1_arg;
int  *result_11;
int  tirar_1_arg;
struct tabla  *result_12;
int *result_13;
int termine_1_arg;
int  terminar_1_arg;
int opcion;
int i;
int turnos;
int nuevos[5];
int puntos;
int tiros;
int anoto[11];
int *jugando;
int indice;
int total;
int bandera=0;

// Dibuja los dados que salieron para mostrar al jugador
void dibujar(int* numbers)
{
	char* arreglo[7];
	arreglo[0] = "╔═══════╗";
	arreglo[1] = "║       ║";
	arreglo[2] = "║●      ║";
	arreglo[3] = "║      ●║";
	arreglo[4] = "║   ●   ║";
	arreglo[5] = "║●     ●║";
	arreglo[6] = "╚═══════╝";

	char* arregloDados[6][3];
	arregloDados[0][0] = arreglo[1];
	arregloDados[0][1] = arreglo[4];
	arregloDados[0][2] = arreglo[1];
	arregloDados[1][0] = arreglo[2];
	arregloDados[1][1] = arreglo[1];
	arregloDados[1][2] = arreglo[3];
	arregloDados[2][0] = arreglo[2];
	arregloDados[2][1] = arreglo[4];
	arregloDados[2][2] = arreglo[3];
	arregloDados[3][0] = arreglo[5];
	arregloDados[3][1] = arreglo[1];
	arregloDados[3][2] = arreglo[5];
	arregloDados[4][0] = arreglo[5];
	arregloDados[4][1] = arreglo[4];
	arregloDados[4][2] = arreglo[5];
	arregloDados[5][0] = arreglo[5];
	arregloDados[5][1] = arreglo[5];
	arregloDados[5][2] = arreglo[5];

	printf("\033[%d;%dm %s %s %s %s %s \n",1,37,arreglo[0],arreglo[0],arreglo[0],arreglo[0],arreglo[0]);

	for(int i=0; i<3; i++)
	{
		for(int j=0; j< 5;j++)
		{
			printf("\033[%d;%dm %s", 1, 37,arregloDados[numbers[j]-1][i]);
		}
		printf("\n");
	}

	printf("\033[%d;%dm %s %s %s %s %s \n ",1,37,arreglo[6],arreglo[6],arreglo[6],arreglo[6],arreglo[6]);
}

//Metodo auxiliar para la busqueda de un dado en el conjunto salido
int buscar(int n, int arr[], int lgth){
	int resu = 0;
	for(int i=0; i<lgth; i++)
		if (arr[i] == n)
			resu = 1;
	return resu;
}

//Controla si salio Escalera en la jugada del jugador
int Escalera(){
	int resu = 0;
	if (buscar(2,result_7->numeros,5) && buscar(3,result_7->numeros,5) && buscar(4,result_7->numeros,5) && buscar(5,result_7->numeros,5) )
		if (buscar(1,result_7->numeros,5) || buscar(2,result_7->numeros,6))
			resu = 1;
	return resu;	
}

//Controla si salio Generala en la jugada del jugador
int Generala(){
	int resu = result_7->numeros[0] == result_7->numeros[1] && result_7->numeros[1] == result_7->numeros[2] && result_7->numeros[2] == result_7->numeros[3] && result_7->numeros[3] == result_7->numeros[4]; 
	
	return resu;
}

//Controla si salio Full en la jugada del jugador
int Full(){
	int resu = 0;
	if (Generala())		//Asumimos que la generala es un caso especial del full
		resu = 1;
	else{
		int repetidos[6];
		int j = 0;
	
		repetidos[0]=repetidos[1]=repetidos[2]=repetidos[3]=repetidos[4]=repetidos[5]=0;
	
		while(j<5){
			repetidos[(result_7->numeros[j])-1]+=1;
			j++;
		}
	
		resu = buscar(2,repetidos,6) && buscar(3,repetidos,6);
	}
	return resu;
}

//Controla si salio Poker en la jugada del jugador
int Poker(){
	int aDevolver = 0;
	int repetidos[6];
	int j = 0;
	
	repetidos[0]=repetidos[1]=repetidos[2]=repetidos[3]=repetidos[4]=repetidos[5]=0;
	
	while(j<5){
		repetidos[(result_7->numeros[j])-1]+=1;
		j++;
	}
	
	j=0;
	while(j<6 && aDevolver==0){
		aDevolver = (repetidos[j] >= 4);
		j++;
	}
		
	return aDevolver;
}

void cargar_argumentos_tachar(int op){

	int i = 0;
	while(i<11){
		if(i == op){
			tachar_1_arg.valores[i] = 1;
		}
		else
			tachar_1_arg.valores[i] = 0;
		i++;
	}
}

void cargar_argumentos_anotar(int op){

	int i = 0;
	while(i<11){
		if(i == op){
			anotar_1_arg.valores[i] = 1;
		}
		else
			anotar_1_arg.valores[i] = 0;
		i++;
	}
}

int VolverATirar(){

	nuevos[0]=-1;
	nuevos[1]=-1;
	nuevos[2]=-1;
	nuevos[3]=-1;
	nuevos[4]=-1;

	int control=1;
	
	//El jugador tiene la posibilidad de volver a tirar la cantidad de datos que quiera
	while (control){
		printf("¿Cuantos dados desea volver a tirar? ");
		scanf("%i",&tiros);
		if (tiros >=1 && tiros<=5)
			control=0;
		else
		{
			printf("Ingrese una opcion valida\n");
			fflush(stdin);
		}
	}
	printf("Los dados se numeran de izquierda a derecha de 1 a 5 \n");
	
	i=0;
	while(i<tiros){
		control=1;
		int op;
		while(control){ 
			switch(i){
				case 0:
					printf("¿Que dado desea tirar primero? ");
				break;
				case 1:
					printf("¿Que dado desea tirar segundo? ");
                break;
				case 2:
                    printf("¿Que dado desea tirar tercero? ");
                break;
				case 3:
                    printf("¿Que dado desea tirar cuarto? ");
                break;
				case 4:
                    printf("¿Que dado desea tirar quinto? ");
                break;
                default:
                break;
			}
			op;
			scanf("%i",&op);
			if (op>0 && op<6)
				control=0;
			else{
                printf("Ingrese una opcion valida\n");
                fflush(stdin);
            }
		}
		opcion=op-1;
		result_11 = tirar_1(&opcion,clnt);
		if (result_11 == (int *) NULL) {
			clnt_perror (clnt, "call failed");
		}
		nuevos[opcion]=*result_11;
		i++;
	}
	i=0;
	while(i<5){
		if(nuevos[i]!=-1){
			result_7->numeros[i]=nuevos[i];
		}
		i++;
	}
	printf("El resultado de tirar los dados seleccionados nuevamente es:\n");
	//Muestro nuevamente los dados en pantalla para facilidad del jugador.
	dibujar(result_7->numeros);
}

//Metodo para tachar la categoria seleccionada en base a los resultados parciales 
int Tachar(){
	int control=1;
	while (control){
		printf("Elija que categoría desea tachar: \n");
		printf("                                 1 - Unos \n");
		printf("                                 2 - Dos \n");
		printf("                                 3 - Tres \n");
		printf("                                 4 - Cuatros \n");
		printf("                                 5 - Cincos \n");
		printf("                                 6 - Seis \n");
		printf("                                 7 - Escalera \n");
		printf("                                 8 - Full \n");
		printf("                                 9 - Poker \n");
		printf("                                10 - Generala \n");
		printf("                                11 - Generala Doble \n");
	
		printf("Ingrese categoría a tachar: ");
		scanf("%i",&opcion);
		
		if (opcion>0 && opcion<12)
			control=0;
		else{
            printf("Ingrese una opcion valida\n");
            fflush(stdin);
        }
	}

	printf("\n\n● Usted va a tachar la categoría %i \n",opcion);

	if (anoto[opcion-1]!=-1){
		
		cargar_argumentos_tachar(opcion-1);
		result_9 = tachar_1(&tachar_1_arg, clnt);
		
		if (result_9 == (int *) NULL) {
			clnt_perror (clnt, "call error");
		}
		anoto[opcion-1]=-1;
		printf("● Ha tachado lo solicitado \n\n");
	}
	else{
		printf("● La categoría elegida ya se encuentra tachada, seleccione otra\n\n");
		Tachar();
	}
}

//Metodo para anotar la categoria seleccionada en base a los resultados parciales 
int Anotar(){
	int pos;
	int control=1;
	while (control){
        printf("Elija que categoría desea anotar: \n");
		printf("                                 1 - Unos \n");
		printf("                                 2 - Dos \n");
		printf("                                 3 - Tres \n");
		printf("                                 4 - Cuatros \n");
		printf("                                 5 - Cincos \n");
		printf("                                 6 - Seis \n");
		printf("                                 7 - Escalera \n");
		printf("                                 8 - Full \n");
		printf("                                 9 - Poker \n");
		printf("                                10 - Generala \n");
		printf("                                11 - Generala Doble \n");
        
		printf("Ingrese categoría a anotar: ");
		scanf("%i",&opcion);
        
        if (opcion>0 && opcion<12)
            control=0;
        else{
            printf("Ingrese una categoría valida\n");
            fflush(stdin);
        }
    }
	
	printf("\n\n● Usted va a anotar la categoría %i \n",opcion);
	
	if(anoto[opcion-1]==0){
		anoto[opcion-1]=1;

		if(opcion<7){
			i=0;
			puntos=0;
			while(i<5){
				if(result_7->numeros[i]==opcion){
					puntos=puntos+opcion;				
				}
				i++;
			}
			printf("● Ha anotado %i puntos al %i \n\n", puntos, opcion);
			if(puntos==0) puntos=-5; //bandera de 0 puntos
			anotar_1_arg.valores[opcion-1]=puntos;

			result_8 = anotar_1(&anotar_1_arg, clnt);
			if (result_8 == (int *) NULL) {
				clnt_perror (clnt, "call failed");
			}
		}
		else{
			switch(opcion){
				case  7:
					if(Escalera()){
						cargar_argumentos_anotar(opcion-1);
						result_8 = anotar_1(&anotar_1_arg, clnt);
						if (result_8 == (int *) NULL) {
							clnt_perror (clnt, "call failed");
						}
						printf("● Ha anotado la escalera \n\n");
					}
					else{
						cargar_argumentos_tachar(opcion-1);
						result_9 = tachar_1(&tachar_1_arg, clnt);
						if (result_9 == (int *) NULL) {
							clnt_perror (clnt, "call failed");
						}
						anoto[opcion-1] = -1;
						printf("● No tiene escalera. Se ha tachado la escalera \n\n");
					}
				break;
				case  8:
					if(Full()){
						cargar_argumentos_anotar(opcion-1);
						result_8 = anotar_1(&anotar_1_arg, clnt);
						if (result_8 == (int *) NULL) {
							clnt_perror (clnt, "call failed");
						}
						printf("● Ha anotado Full \n\n");
					}
					else{
						cargar_argumentos_tachar(opcion-1);
						result_9 = tachar_1(&tachar_1_arg, clnt);
						if (result_9 == (int *) NULL) {
							clnt_perror (clnt, "call failed");
						}
						anoto[opcion-1] = -1;
						printf("● No tiene Full. Se ha tachado el Full \n\n");
					}
				break;
				case  9:
					if(Poker()){
						cargar_argumentos_anotar(opcion-1);
						result_8 = anotar_1(&anotar_1_arg, clnt);
						if (result_8 == (int *) NULL) {
							clnt_perror (clnt, "call failed");
						}
						printf("● Ha anotado el Poker \n\n");
					}
					else{
						cargar_argumentos_tachar(opcion-1);
						result_9 = tachar_1(&tachar_1_arg, clnt);
						if (result_9 == (int *) NULL) {
							clnt_perror (clnt, "call failed");
						}
						anoto[opcion-1] = -1;
						printf("● No tiene Poker. Se ha tachado el Poker \n\n");
					}
				break;
				case  10:
					if(Generala()){
						cargar_argumentos_anotar(opcion-1);
						result_8 = anotar_1(&anotar_1_arg, clnt);
						if (result_8 == (int *) NULL) {
							clnt_perror (clnt, "call failed");
						}
						printf("● Ha anotado la Generala \n\n");
					}
					else{
						cargar_argumentos_tachar(opcion-1);
						result_9 = tachar_1(&tachar_1_arg, clnt);
						if (result_9 == (int *) NULL) {
							clnt_perror (clnt, "call failed");
						}
						anoto[opcion-1] = -1;
						printf("● No tiene Generala. Se ha tachado la Generala \n\n");
					}
				break;
				case  11:
					if(Generala() && (anoto[9]==1)){
						cargar_argumentos_anotar(opcion-1);
						result_8 = anotar_1(&anotar_1_arg, clnt);
						if (result_8 == (int *) NULL) {
							clnt_perror (clnt, "call failed");
						}
						printf("● Ha anotado la generala doble \n\n");
					}
					else{
						//tachar_1_arg.valores[opcion-1]=1;
						cargar_argumentos_tachar(opcion-1);
						result_9 = tachar_1(&tachar_1_arg, clnt);
						if (result_9 == (int *) NULL) {
							clnt_perror (clnt, "call failed");
						}
						anoto[opcion-1] = -1;
						printf("● No tiene generala. Se ha tachado la generala doble \n\n");
					}
				break;
			}
		}
		anotar_1_arg.valores[opcion-1]=0;
	}
	else{
		if(anoto[opcion-1]==1){
			printf("● Ya ha anotado esa opcion \n\n");
		}
		else{
			printf("● Ha tachado esa opcion \n\n");
		}
		Anotar();
		return 0;
	}
}

//Muestra los resultados obtenidos parcialmente por el jugador correspondiente
void Mostrar(){

	printf("┌───────────────────────────────┐\n");
	printf("│  categoría     │ resultado    │\n");
	printf("│───────────────────────────────│\n");                          
	result_6 = resultadosparciales_1((void*)&resultadosparciales_1_arg, clnt);
	if (result_6 == (char **) NULL) {
		clnt_perror (clnt, "call failed");
	}
	
	int aux[total];
	indice=0;
	//Jugador correspondiente
	(*result_6)= (*result_6) + (*result_1*11);
	
	while(indice<11){
		aux[indice]=**result_6  - 65;
		(*result_6)++;
		switch (indice){
			case 0:
				printf("│ 1              │");
			break;
			case 1:
				printf("│ 2              │");
			break;
			case 2:
				printf("│ 3              │");
			break;
			case 3:
				printf("│ 4              │");
			break;
			case 4:
				printf("│ 5              │");
			break;
			case 5:
				printf("│ 6              │");
			break;
			case 6:
				printf("│ Escalera       │");
			break;
			case 7:
				printf("│ Full           │");
			break;
			case 8:
				printf("│ Poker          │");
			break;
			case 9:
				printf("│ Generala       │");
			break;
			case 10:
				printf("│ Generala Doble │");
			break;
		}

		if(aux[indice]>=0){
			if(aux[indice]<=9)
				printf("%i             │\n",aux[indice]);
			else
				printf("%i            │\n",aux[indice]);
		}
		else
			printf("X             │\n");

		indice++;
	}
	printf("└───────────────────────────────┘\n");
}

//Muestra los resultados obtenidos finales por el jugador correspondiente
int MostrarResultados(int puntaje, int gano){

	result_6 = resultadosparciales_1((void*)&resultadosparciales_1_arg, clnt);
	if (result_6 == (char **) NULL) {
		clnt_perror (clnt, "call failed");
	}

	printf("═══════════════════════════════════════════════════════════════════════════\n");
	printf("═══════════════════════════Resultados Parciales:═══════════════════════════\n");
	int aux[total];
	indice=0;
	if(*result_1==0){
		printf("Usted: \n");
		if(puntaje>=0 && gano >=0){
			printf("Su puntaje es %i \n",puntaje);
			if(gano){
				printf("Usted ha ganado el juego :D \n");
			}
			else{
				printf("Usted no ha ganado el juego :( \n");
			}
		}
	}
	else{
		printf("Jugador 0: \n");
	}
	while(indice<total){
		aux[indice]=**result_6 - 65;
		(*result_6)++;
		if(indice !=0){
			if(indice % 11 == 0){
				if(indice/11 == *result_1){
					printf("______________________________________________________________________\n");
					printf("Usted: \n");
					if(puntaje>=0 && gano >=0){
						printf("Su puntaje es %i \n",puntaje);
						if(gano){
							printf("Usted ha ganado el juego :D \n");
						}
						else{
							printf("Usted no ha ganado el juego :( \n");
						}
					}
				}
				else{

					printf("______________________________________________________________________\n");
					printf("Jugador %i \n",indice/11);
				}
			}
		}
		
		switch (indice % 11){
			case 0:
				printf(" Unos            ");
				break;
			case 1:
				printf(" Dos             ");
				break;
			case 2:
				printf(" Tres            ");
				break;
			case 3:
				printf(" Cuatros         ");
				break;
			case 4:
				printf(" Cincos          ");
				break;
			case 5:
				printf(" Seis            ");
				break;
			case 6:
				printf(" Escalera        ");
				break;
			case 7:
				printf(" Full            ");
				break;
			case 8:
				printf(" Poker           ");
				break;
			case 9:
				printf(" Generala        ");
				break;
			case 10:
				printf(" Generala Doble  ");
				break;
		}
		if( aux[indice]>=0){
			printf("%i \n",aux[indice]);
		}
		else{
			printf("X \n");
		}
		indice++;
	}
	printf("═══════════════════════════════════════════════════════════════════════════\n");
}



int main (int argc, char *argv[])
{
	char *host;
	jugando=(int *)malloc(sizeof(int));
	*jugando=-1;
	opcion=0;
	i=0;
	turnos=0;
	nuevos[5];
	puntos=0;
	tiros=0;
	while(i<11){
		anoto[i]=0;
		i++;
	}
	i=0;
	if (argc < 1) {
		printf ("usage: %s server_host\n", argv[0]);
		exit (1);
	}
	//Asumo localhost
	char *auxiliar="localhost";
	if (argc==1)
		host=auxiliar;
	else
		host = argv[1];
	printf("                   ╔═══════════════╗                    \n");
	printf("╔╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦ Bienvenido a la ╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╦╗\n");
	printf("╚╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩    Generala     ╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╩╝\n");
	printf("                   ╚═══════════════╝                    \n");

	turnos=0;
	clnt = clnt_create (host, GENERALA, GENERALAVERS, "udp");
	if (clnt == NULL) {
		clnt_pcreateerror (host);
		exit (1);
	}

	//Se anota en el servidor
	result_1 = anotarse_1((void*)&anotarse_1_arg, clnt);
	if (result_1 == (int *) NULL) {
		clnt_perror (clnt, "call failed");
	}
	int control=1;
        while (control){
		printf("Usted es el jugador número %i.\n\n Elija una opcion:\n 0-Esperar a que otro inicie el juego\n 1-Iniciar el juego\n\nOpcion: ", *result_1);
	        scanf("%i",&opcion);
                if (opcion==0 || opcion==1)
      	        	control=0;
                else
                {
                        printf("Ingrese una opcion valida\n");
                        fflush(stdin);
                }
        }
	if(opcion==1){
		result_2 = empezar_1((void*)&empezar_1_arg, clnt);
		if (result_2 == (int *) NULL) {
			clnt_perror (clnt, "call failed");
		}
		if(*result_2==0){
			printf("No puede empezar el juego porque solo hay un jugador. Cuando otro inicie, podra empezar a jugar \n");
		}
	}
	else{
		printf("Esperando que otro jugador inicie el juego... \n");
	}

	//Debo saber cuantos jugadores hay
	while(*jugando==-1){
		jugando = jugadores_1((void*)&jugadores_1_arg, clnt);
		if (jugando == (int *) NULL) {
			clnt_perror (clnt, "call failed");
		}
	}
	total=*jugando*11;
	printf("═══════════════════════════════════════════════════════════════════════════\n");
	printf("¡Ahora sí! Que comience el juego ... \n\n");
	//Comienza el juego
	while(turnos<11){
		result_4 = permiso_1(result_1, clnt);
		if (result_4 == (int *) NULL) {
			clnt_perror (clnt, "call failed");
		}

		if(*result_4==1){
			turnos++;

			//Primero mostrare la jugada anterior y los resultados parciales
			result_5 = jugadaanterior_1((void*)&jugadaanterior_1_arg, clnt);
			if (result_5 == (struct jugada *) NULL) {
				clnt_perror (clnt, "call failed");
			}

			i=0;
			if(result_5->anoto<11){
				printf("Ultima jugada del oponente: \n");
				dibujar(result_5->dados);
				printf("\n");
				switch(result_5->anoto){
					case 0:
						printf("Anoto los Uno \n");
						break;
					case 1:
						printf("Anoto los Dos \n");
						break;
					case 2:
						printf("Anoto los Tres \n");
						break;
					case 3:
						printf("Anoto los Cuatro \n");
						break;
					case 4:
						printf("Anoto los Cinco \n");
						break;
					case 5:
						printf("Anoto los Seis \n");
						break;
					case 6:
						printf("Anoto Escalera \n");
						break;
					case 7:
						printf("Anoto Full \n");
						break;
					case 8:
						printf("Anoto Poker \n");
						break;
					case 9:
						printf("Anoto Generala \n");
						break;
					case 10:
						printf("Anoto Generala Doble \n");
						break;
					case -1:
						printf("Tacho los Dos \n");
						break;
					case -2:
						printf("Tacho los Tres \n");
						break;
					case -3:
						printf("Tacho los Cuatro \n");
						break;
					case -4:
						printf("Tacho los Cinco \n");
						break;
					case -5:
						printf("Tacho los Seis \n");
						break;
					case -6:
						printf("Tacho Escalera \n");
						break;
					case -7:
						printf("Tacho Full \n");
						break;
					case -8:
						printf("Tacho Poker \n");
						break;
					case -9:
						printf("Tacho Generala \n");
						break;
					case -10:
						printf("Tacho Generala Doble \n");
						break;
				}
				/////////////////////////////////////////////////////////////////////////////////
				/*
				result_6 = resultadosparciales_1((void*)&resultadosparciales_1_arg, clnt);
				if (result_6 == (char **) NULL) {
					clnt_perror (clnt, "call failed");
				}
				MostrarResultados(-1,-1);
				*/
				/////////////////////////////////////////////////////////////////////////////////

			}
			else{
				printf("Usted arrancara el juego! \n");
			}
			printf("\n═══════════════════════════════════════════════════════════════════════════\n");
		    printf("\n");
		    Mostrar();
			printf("Es su turno. Se tiraran los dados... \n");
			result_7 = jugar_1((void*)&jugar_1_arg, clnt);

			if (result_7 == (struct dados *) NULL) {
				clnt_perror (clnt, "call failed");
			}
			
			printf("Han salido los dados: \n");
			dibujar(result_7->numeros);			
			printf("\n");
			control=1;
			while (control){
				printf("Elija una opcion:\n0-Volver a tirar\n1-Anotar\n2-Tachar\n3-Resultados Parciales\n4-Ver Dados\nOpcion: ");
				scanf("%i",&opcion);
				if (opcion==0 || opcion==1 || opcion==2 || opcion==3 || opcion==4){
					if (opcion==3)
						MostrarResultados(-1,-1);
					else
						if (opcion==4)
							dibujar(result_7->numeros);
						else
							control=0;
				}
				else
				{
                        		printf("Ingrese una opcion valida\n");
                        		fflush(stdin);
                		}
			}
			switch (opcion){
				case 0:
					VolverATirar();
					control=1;
					while (control){
						printf("Elija una opcion:\n0-Volver a tirar\n1-Anotar\n2-Tachar\n3-Resultados Parciales\n4-Ver Dados\nOpcion: ");
						scanf("%i",&opcion);
						if (opcion==0 || opcion==1 || opcion==2 || opcion==3 || opcion==4){
							if (opcion==3)
								MostrarResultados(-1,-1);
							else
								if (opcion==4)
									dibujar(result_7->numeros);
								else
									control=0;
						}
						else
						{
				                        printf("Ingrese una opcion valida\n");
                        				fflush(stdin);
                				}
					}
					switch (opcion){
						case 0:
							VolverATirar();
							control=1;
							while (control){
								printf("Elija una opcion:\n1-Anotar\n2-Tachar \n3-Resultados Parciales\n4-Ver Dados\nOpcion: ");
								scanf("%i",&opcion);
								if (opcion==1 || opcion==2 || opcion==3 || opcion==4){
									if (opcion==3)
										MostrarResultados(-1,-1);
									else
										if (opcion==4)
											dibujar(result_7->numeros);
										else
											control=0;
								}
								else
								{
                        						printf("Ingrese una opcion valida\n");
						                        fflush(stdin);
                						}
							}
							switch (opcion){
								case 1:
									Anotar();
									break;
								case 2:
									Tachar();
									break;
							}
							break;
						case 1:
							Anotar();
							break;
						case 2:
							Tachar();
							break;
					}
					break;
				case 1:
					Anotar();
					break;
				case 2:
					Tachar();
					break;
			}

			//Indico que termino mi turno
			result_4 = permiso_1(result_1, clnt);
			if (result_4 == (int *) NULL) {
				clnt_perror (clnt, "call failed");
			}
			printf("»» Es el turno de otro jugador \n");
			printf("\n═══════════════════════════════════════════════════════════════════════════\n");
                        printf("\n");
			bandera=1;
		}
		else
			if (bandera==0)
			{
				printf("Otro jugador inicio primero\n");
                        	printf("\n═══════════════════════════════════════════════════════════════════════════\n");
                        	printf("\n");
				bandera=1;
			}
	}

	result_13 = (int *)malloc(sizeof(int));
	*result_13=0;

	while(*result_13==0){
		result_13 = termine_1(result_1, clnt);
		if (result_13 == (int *) NULL) {
			clnt_perror (clnt, "call failed");
		}
	}
	printf("Ha terminado el juego \n");
	printf("\n═══════════════════════════════════════════════════════════════════════════\n");
        printf("\n");
	result_12 = terminar_1(result_1, clnt);
	if (result_12 == (struct tabla *) NULL) {
		clnt_perror (clnt, "call failed");
	}

	printf("Resultados Finales: \n");
	result_6 = resultadosparciales_1((void*)&resultadosparciales_1_arg, clnt);
	if (result_6 == (char **) NULL) {
		clnt_perror (clnt, "call failed");
	}
	MostrarResultados(result_12->puntaje, result_12->gano);
	clnt_destroy (clnt);

printf("\033[0m"); //Resets the text to default color
exit (0);
}
