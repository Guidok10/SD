
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/sysinfo.h>
#include <unistd.h>	
#include <strings.h>
#include <netdb.h>
#include <limits.h>
//para sockets
#include <sys/socket.h>
//para el calculo del tiempo
#include <sys/time.h>
#include <time.h>


//operaciones:
#define AL_AZAR 1
#define ORDENADA 2
#define MISMAS 3
//def de componente:
#define CANT_PALABRAS 10667 //cada nucleo: 10667*12= 128000 aprox.
#define NRO_NUCLEOS 4     //si la pc tiene 4 nucleos: 10667*4= 42668 apox.

#define MAX_PALABRAS_AL_AZAR 20000   //genero 20000 palabras ordenadas y 20000 aleatorias
#define LENGTH_WORD 10  //tamano de la palbra en caracteres

//variable en la que voy a obtener las palabras
int salida[128000][10];
int UltPosPoolA=-1;
int UltPosPoolO=-1;
char text[10]="abcdefghij";

void *print_message_function( void *ptr );

//Para Socket
#define PORT 13131 // El Puerto Abierto del nodo remoto 
int fd; // ficheros descriptores
size_t numbytes;
struct hostent *he; // estructura que recibirá información sobre el nodo remoto 
struct sockaddr_in server; // información sobre la dirección del servidor


//arreglo que almacena todas las palabras a escribir
char palabrasNodo[CANT_PALABRAS*NRO_NUCLEOS][LENGTH_WORD+1]; //ver como inicializar dentro del main

//pool de palabras al azar:
char palabrasAleatorias[MAX_PALABRAS_AL_AZAR][LENGTH_WORD+1];

//pool de palabras ordenadas:
char palabrasOrdenadas[MAX_PALABRAS_AL_AZAR][LENGTH_WORD+1];

//estructura que se envia a cada uno de los hilos.
typedef struct Gen{
    int tipoOperacion;
    int posIni;
    int posFin;
}*propsGen;

void generarRangoAlAzar(int posInicio, int posFin){
    int posRandomWord;
    while (UltPosPoolA == -1){}
    for(int i= posInicio; i<=posFin; i++){		
        posRandomWord= rand()%(UltPosPoolA+1);
        //palabrasNodo[i]= palabrasAleatorias[posRandomWord];
        sprintf(palabrasNodo[i],"%s",palabrasAleatorias[posRandomWord]);
    }
}

void generarRangoOrdenado(int posInicio, int posFin){
    //asumo que posInicio < posFin y que el rango <= length de palabrasOrdenadas
    int cantElem= posFin-posInicio;
    while (UltPosPoolO < cantElem){}
    int posPrimerWord= (rand()%UltPosPoolO+1)-(cantElem);
    if(posPrimerWord<0)
        posPrimerWord=0;
    int posEscritura=posInicio;
    int contadorEscrituras=0;
    int posLectura=posPrimerWord;
   
    
    while(contadorEscrituras<=cantElem){
        //palabrasNodo[posEscritura]= palabrasOrdenadas[posLectura];
        sprintf(palabrasNodo[posEscritura],"%s",palabrasOrdenadas[posLectura]);
        posEscritura++;
        contadorEscrituras++;
        if(posLectura>=MAX_PALABRAS_AL_AZAR)
            posLectura=0;
        else posLectura++;
    }
}

void generarMismasPalabras(int posInicio, int posFin){
    int posRandomWord=rand()%MAX_PALABRAS_AL_AZAR;
    for(int i= posInicio; i<=posFin; i++){
        //palabrasNodo[i]= palabrasAleatorias[posRandomWord];
        sprintf(palabrasNodo[i],"%s",palabrasAleatorias[posRandomWord]);
    }
}
//hilos
void * generarRangoPalabras(void * tipoGenerador){

    propsGen opcion; 
    opcion = (propsGen) tipoGenerador;
    int tipo= opcion->tipoOperacion;
    if(tipo==AL_AZAR){
        generarRangoAlAzar(opcion->posIni, opcion->posFin);
    }
    else if (tipo== ORDENADA){
        generarRangoOrdenado(opcion->posIni, opcion->posFin);
    }
    else if (tipo== MISMAS){
        generarMismasPalabras(opcion->posIni,opcion->posFin);
    }
    int *salida=malloc(sizeof(int));
        *salida=1;
        return salida;
}

void * llenarPoolPalabrasAleatorias(){
    char randomWord[LENGTH_WORD+1];
    char letra;
    for(int posPalabra=0; posPalabra<MAX_PALABRAS_AL_AZAR; posPalabra++){
        for(int i =0; i<LENGTH_WORD; i++){
            //genero nro aleatorio y obtengo su char correspondiente..
            letra= ((rand()%26)+97);
            if(i==0)
                sprintf(randomWord,"%c",letra);
            else 
                sprintf(randomWord,"%s%c",randomWord,letra);
        }
        sprintf(palabrasAleatorias[posPalabra], "%s", randomWord);
        UltPosPoolA=posPalabra;
    }
	int *salida=malloc(sizeof(int));
	*salida=1;
	return salida;
}
void * llenarPoolPalabrasOrdenadas(){
    char randomWord[LENGTH_WORD+1];
    int incremento= 7;
    int flagSuma=0;

    //genero la primer palabra aleatoria
    char letra;
    for(int i =0; i<LENGTH_WORD; i++){
            //genero nro aleatorio y obtengo su char correspondiente..
            letra= ((rand()%26)+97);
            if(i==0)
                sprintf(randomWord,"f");
            else 
                sprintf(randomWord,"%s%c",randomWord,letra);
        }
    //agrego la primer palabra..
    sprintf(palabrasOrdenadas[0], "%s", randomWord);
    //agrego el resto de las palabras segun el incremento..
    int aux;
    for(int posPalabra=1; posPalabra<MAX_PALABRAS_AL_AZAR; posPalabra++){
        //genero nro aleatorio y obtengo su char correspondiente.. 
        for(int i=LENGTH_WORD-1; i>=0; i--){
            aux= randomWord[i];
            if(i==LENGTH_WORD-1)
                aux+=incremento;
            //chequeo flag suma
            if(flagSuma){
                aux++;
                flagSuma=0;
            }
            //chequeo overflow del digito
            if(aux>122){
                aux= 97;
                flagSuma=1;
            }
            randomWord[i]= aux;
        }
        sprintf(palabrasOrdenadas[posPalabra], "%s", randomWord);
        UltPosPoolO=posPalabra;
        }
        int *salida=malloc(sizeof(int));
        *salida=1;
        return salida;
}
int main(int argc, char *argv[])

{
	//obtengo de consola la direccion ip del servidor de socket
	if (argc !=2) {
      printf("Uso: %s <Dirección IP>\n",argv[0]);
      exit(-1);
   }

   if ((he=gethostbyname(argv[1]))==NULL){
      printf("gethostbyname() error\n");
      exit(-1);
   }

   if ((fd=socket(AF_INET, SOCK_STREAM, IPPROTO_TCP))==-1){
      printf("socket() error\n");
      exit(-1);
   }
	int a = 470000;
	if ( (setsockopt(fd, SOL_SOCKET, SO_SNDBUF, &a ,sizeof(int)) ) < 0 )
	{
		perror("setsockopt--> seteo de buffer de recepcion de socket");  
        exit(EXIT_FAILURE); 
	}
   server.sin_family = AF_INET;
   server.sin_port = htons(PORT);
   server.sin_addr = *((struct in_addr *)he->h_addr);
   bzero(&(server.sin_zero),8);
   //me conecto al socket del servidor 
   if(connect(fd, (struct sockaddr *)&server,sizeof(struct sockaddr))==-1){
      printf("connect() error\n");
      exit(-1);
  }
   char *msg= malloc(sizeof(char)*20); 
   //espero a un mensaje para iniciar la generacion
   if ((numbytes=recv(fd,msg,4,0)) < 0){
      	printf("Error en recv() \n");
    	exit(-1);
   	}
   	printf("Arranque a Generar \n");
	srand(time(NULL));
	//para caluclar el tiempo
    struct timeval tv1, tv2;
    
    gettimeofday(&tv1,NULL);
    //Creo los hilos para llenar los pools de palabras
    pthread_t poolA,poolO;
    pthread_create( &poolA, NULL,llenarPoolPalabrasAleatorias , NULL);
    pthread_create( &poolO, NULL,llenarPoolPalabrasOrdenadas , NULL);
    
    
    
    
	//Me fijo la cantidad de procesadores del sistema
	int cantCpus=0;
	cantCpus= NRO_NUCLEOS;//get_nprocs_conf(); //PONER
	//compruebo de que sean mayor a 0
	if(cantCpus<=0)
		exit(-1);	
	//creo un arreglo de los threads
	pthread_t threads[cantCpus];  

    //Creo los hilos que me generaran las palabras
    struct Gen datos[cantCpus];
    int i=0;
    while(i<cantCpus){
		
		if(i%2!=0)
			datos[i].tipoOperacion=AL_AZAR;
		else
			datos[i].tipoOperacion=ORDENADA;
			
		datos[i].posIni=CANT_PALABRAS*i;
		datos[i].posFin=datos[i].posIni+(CANT_PALABRAS-1);
		pthread_create( &(threads[i]), NULL, generarRangoPalabras, &datos[i]);
		i++;
	} 
    //Espero a que todos los hilos finalicen.
    
	i=0;
	while(i<cantCpus){
		pthread_join( threads[i], NULL);
		i++;
	}
	//Envio al servidor las palabras generadas
		if((send(fd,palabrasNodo,sizeof(palabrasNodo),0))!=sizeof(palabrasNodo)){
			printf("Error al comparar tamaño enviado con tamaño local ----->>METODO SEND\n");
			exit(0);
		}
	

    gettimeofday(&tv2,NULL);
    
    printf ("Total time = %f seconds\n",
         (double) (tv2.tv_usec - tv1.tv_usec) / 1000000 +
         (double) (tv2.tv_sec - tv1.tv_sec));
	printf("Termine de Generar!! xD\n");
    //Cierro socket 
    close(fd); 
    

}


