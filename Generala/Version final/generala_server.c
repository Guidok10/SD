#include "generala.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>

int usuarios=0;
int id=0;
struct tabla *tablas;
int punteroSiguiente=-1;
int jugando=-2;
struct jugada Anterior;
int * terminaron;
int terminados=0;
int jugadas;
int llamadas;

int *anotarse_1_svc(void *argp, struct svc_req *rqstp)
{
	static int  result;
	if(jugando==-2){
		printf("Se ha anotado un nuevo jugador\n");
		result=id;
		usuarios++;
		id++;
	}
	else{
		result=-1;
	}

	return &result;
}

int *empezar_1_svc(void *argp, struct svc_req *rqstp)
{
	static int  result;
	int i;
	int j;
	punteroSiguiente=-1;
	if(usuarios>1 && jugando==-2){
		punteroSiguiente=0; //Empieza el primero que se anoto
		result=1;
		jugando=-1; //No hay nadie jugando

		tablas=(struct tabla *)malloc(usuarios*sizeof(struct tabla));
		i=0;
		j=0;
		llamadas=0;
		while(i<usuarios){
			while(j<11){
				tablas[i].valores[j]=0;
				j++;
			}
			tablas[i].gano=0;
			tablas[i].puntaje=0;
			i++;
			j=0;
		}
		terminaron=(int *)malloc(usuarios*sizeof(int));
		jugadas=-1;

		Anterior.dados[0]=0;
		Anterior.dados[1]=0;
		Anterior.dados[2]=0;
		Anterior.dados[3]=0;
		Anterior.dados[4]=0;
		Anterior.anoto=11;

		i=0;
		while(i<usuarios){
			terminaron[i]=0;
			i++;
		}
	}
	else{
		if(jugando>-2){
			result=1; //Ya han empezado el juego
			printf("El juego ha comenzado\n");
		}
		else{ 
			result=0; //Debe esperar mas jugadores
			printf("Esperando por mas jugadores\n");
		}
	}

	return &result;
}

int *jugadores_1_svc(void *argp, struct svc_req *rqstp)
{
	static int  result;

	result=-1;
	if(jugando!=-2){
		result=usuarios; //Ha comenzado el juego, no puede anotarse mas nadie
	}

	return &result;
}

int *permiso_1_svc(int *argp, struct svc_req *rqstp)
{
	static int  result;
	result=0;
	if(jugando==-1){
		//No hay nadie jugando
		if(punteroSiguiente== *argp){
			//El que invoca es quien debe jugar
			result=1;
			llamadas=0;
			jugando=*argp; //Ahora hay alguien jugando
			jugadas++;
		}
		else{
			llamadas++;
			if(llamadas>1000){
				//Se asume que el jugador se ha caido
				punteroSiguiente= (punteroSiguiente+1)%usuarios;
				llamadas=0;
			}
			result=0;
		}
	}
	else{
		if(jugando==*argp){
			//El que esta jugando es el que llamo a la funcion
			punteroSiguiente= (punteroSiguiente+1)%usuarios;
			result=1;
			jugando=-1; //No hay nadie jugando
		}

	}
	return &result;
}

struct jugada *jugadaanterior_1_svc(void *argp, struct svc_req *rqstp)
{
	return &Anterior;
}

char **resultadosparciales_1_svc(void *argp, struct svc_req *rqstp)
{
	static char * result;

	int j; //para recorrer jugadores
	int p; //para recorrer puntajes
	int i; //para recorrer el string
	i=0;

	result=(char *)malloc(usuarios*11*sizeof(char));
	for(j=0;j<usuarios;j++){
		for(p=0;p<11;p++){
			result[i]=(char)(65+tablas[j].valores[p]);
			i++;
		}
	}

	return &result;
}



struct dados *jugar_1_svc(void *argp, struct svc_req *rqstp)  
{
	static struct dados  result;

	srand(time(NULL));
	result.numeros[0]=(rand()%6)+1;
	result.numeros[1]=(rand()%6)+1;
	result.numeros[2]=(rand()%6)+1;
	result.numeros[3]=(rand()%6)+1;
	result.numeros[4]=(rand()%6)+1;

	Anterior.dados[0]=result.numeros[0];
	Anterior.dados[1]=result.numeros[1];
	Anterior.dados[2]=result.numeros[2];
	Anterior.dados[3]=result.numeros[3];
	Anterior.dados[4]=result.numeros[4];

	return &result;
}

int *anotar_1_svc(struct tabla *argp, struct svc_req *rqstp)
{
	static int  result;
	int i;
	int encontre;
	int aux;
	i=-1;
	encontre=0;
	aux=0;

	while(!encontre){
		i++;
		aux=argp->valores[i];
		encontre=aux!=0;
	}

	if(i<6){
		//Es un numero
		if(aux==-5) 
			aux=0;
		tablas[punteroSiguiente].valores[i]=aux; //Puntero siguiente aun es el jugador actual
	}
	else{
		switch(i)
		{
			case 6:
				tablas[punteroSiguiente].valores[i]=25; //Escalera --> anda
				break;
			case 7:
				tablas[punteroSiguiente].valores[i]=35; //Full --> anda
				break;
			case 8:
				tablas[punteroSiguiente].valores[i]=45; //Poker --> anda
				break;
			case 9:
				tablas[punteroSiguiente].valores[i]=50; //Generala --> anda
				break;
			case 10:
				tablas[punteroSiguiente].valores[i]=60; //Generala Doble
				break;
		}
	}
	Anterior.anoto=i;
	i=0;

	result=1;
	return &result;
}

int *tachar_1_svc(struct tabla *argp, struct svc_req *rqstp)
{
	static int  result;

	int i=-1;
	int encontre=0;
	int aux;
	
	while(!encontre){
		i++;
		aux=argp->valores[i];
		encontre=aux!=0;
	}

	tablas[punteroSiguiente].valores[i]=-1;
	Anterior.anoto=i*(-1);

	return &result;
}

int *salir_1_svc(int *argp, struct svc_req *rqstp)
{
	static int  result;
	result=0;
	return &result;
}

int *tirar_1_svc(int *argp, struct svc_req *rqstp)
{
	static int  result;

	result=(rand()%6)+1;
	Anterior.dados[*argp]=result;
	return &result;
}

struct tabla *terminar_1_svc(int *argp, struct svc_req *rqstp)
{
	static struct tabla  result;
	int a;
	int j;
	int puntos;
	int puntosMaximos;
	int sumaActual;
	puntos=0;
	sumaActual=0;
	j=0;
	a=0;
	puntosMaximos=0;
	
	while(j<11){
		result.valores[j]=0;
		j++;
	}
	result.gano=0;
	result.puntaje=0;
	j=0;
	while(a<usuarios){
		while(j<11){
			if(tablas[a].valores[j]<0) tablas[a].valores[j]=0;
			sumaActual=sumaActual+tablas[a].valores[j];
			if(a==*argp){
				result.valores[j]=tablas[a].valores[j];
			}
			j++;
		}
		if(a==*argp){
			puntos=sumaActual;
		}
		if(sumaActual>puntosMaximos){
			puntosMaximos=sumaActual;
		}
		sumaActual=0;
		a++;
		j=0;
	}
	result.puntaje=puntos;
	result.gano=(puntos==puntosMaximos);
	return &result;
}
int *termine_1_svc(int *argp, struct svc_req *rqstp)
{
	static int  result;
	result=0;
	if(terminaron[*argp]==0){
		terminaron[*argp]=1;
		terminados++;
	}
	if(terminados==usuarios){
		result=1;
	}

	return &result;
}
