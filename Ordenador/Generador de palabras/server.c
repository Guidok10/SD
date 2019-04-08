#include <stdio.h> 
#include <string.h>    
#include <stdlib.h> 
#include <errno.h> 
#include <unistd.h>  
#include <arpa/inet.h>    
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO 
//para el calculo del tiempo
#include <sys/time.h>
#include <time.h>
//Threads
#include <pthread.h>

//socket
#define TRUE   1 
#define FALSE  0 
#define PORT 13131

short cantConexiones=0;

//operaciones:
#define AL_AZAR 1
#define ORDENADA 2
#define MISMAS 3

#define MAX_PALABRAS_AL_AZAR 20000   //genero 20000 palabras ordenadas y 20000 aleatorias
#define LENGTH_WORD 10  //tamano de la palbra en caracteres


#define CANT_PALABRAS 10667 //cada nucleo: 10667*12= 128000 aprox.
#define NRO_NUCLEOS 4      //si la pc tiene 4 nucleos: 10667*4= 42668 apox.

//variable en la que voy a obtener las palabras
int palabrasGeneradas=0;
int UltPosPoolA=-1;
int UltPosPoolO=-1;
char text[10]="abcdefghij";

void *print_message_function( void *ptr );

//arreglo que almacena todas las palabras a escribir
char palabrasNodo[CANT_PALABRAS*NRO_NUCLEOS][11];
 
//pool de palabras al azar:
char palabrasAleatorias[MAX_PALABRAS_AL_AZAR][LENGTH_WORD+1];

//pool de palabras ordenadas:
char palabrasOrdenadas[MAX_PALABRAS_AL_AZAR][LENGTH_WORD+1];

//para calcular el tiempo
struct timeval tv1, tv2;

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
void escribirArchivo(){
	FILE * f;
	f= fopen("archivo_azul", "a"); //el atributo "a" indica que hago append al archivo de texto
	int cantWordLinea=0;
	int i=0;
	while(i<((CANT_PALABRAS*NRO_NUCLEOS)-1)&&palabrasGeneradas!=128000){
		if(palabrasGeneradas==127999){
			fprintf(f, "%s\n", palabrasNodo[i]);
		}else{
			if(cantWordLinea==(CANT_PALABRAS*NRO_NUCLEOS)-2){
				fprintf(f, "%s\n", palabrasNodo[i]);
				cantWordLinea=0;
			}
			else{
				fprintf(f, "%s ", palabrasNodo[i]);
				
				cantWordLinea++;
			}
		}
		palabrasGeneradas++;
		i++;
	}
	if(palabrasGeneradas == 128000){
		gettimeofday(&tv2,NULL);
		printf ("Total time = %ld microseconds\n",
          (tv2.tv_usec - tv1.tv_usec) +
          (tv2.tv_sec - tv1.tv_sec) * 1000000);
         fclose(f);
         printf("Archivo Generado Exitosamente!!\n");
         exit(0);
	 }
	fclose(f);
}
void generadorLocal(){
	srand(time(NULL));
	
    
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
    
	
    escribirArchivo();//almaceno las palabras en un archivo

    
    
}

int main(int argc , char *argv[])  
{  
    int opt = TRUE;  
    int master_socket , addrlen , new_socket , client_socket[2] , 
          max_clients = 2 , activity, i , valread , sd;  
    int max_sd;  
    struct sockaddr_in address;  
    //printf("El timeout del socket es %d\n",SO_RCVTIMEO);
    //set of socket descriptors 
    fd_set readfds;
    
    //inicializo todos los client_socket[] a 0 
    for (i = 0; i < max_clients; i++)  
    {  
        client_socket[i] = 0;  
    }  
        
    //creo el socket master  
    if( (master_socket = socket(AF_INET , SOCK_STREAM , 0)) == 0)  
    {  
        perror("socket failed");  
        exit(EXIT_FAILURE);  
    }  
    
    //set master socket to allow multiple connections , 
    //this is just a good habit, it will work without this 
    if( setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, 
          sizeof(opt)) < 0 )  
    {  
        perror("setsockopt");  
        exit(EXIT_FAILURE);  
    }  
    int a = 470000*2; //el arreglo ocupa 469348 bytes aprox 470000, y como quiero recibir 2, AMPLIO EL BUFFER del socket al doble para evitar retrasos en el envio
	if ( (setsockopt(master_socket, SOL_SOCKET, SO_RCVBUF, &a ,sizeof(int)) ) < 0 )
	{
		perror("setsockopt--> seteo de buffer de recepcion de socket");  
        exit(EXIT_FAILURE); 
	}
    
    //tipo de socket creado
    address.sin_family = AF_INET;  
    address.sin_addr.s_addr = INADDR_ANY;  
    address.sin_port = htons( PORT );  
        
    //bind the socket al port
    if (bind(master_socket, (struct sockaddr *)&address, sizeof(address))<0)  
    {  
        perror("bind failed");  
        exit(EXIT_FAILURE);  
    }  
    printf("Listener on port %d \n", PORT);  
        
    //Especifico la maxima cantidad de conexiones
    if (listen(master_socket, 2) < 0)  
    {  
        perror("listen");  
        exit(EXIT_FAILURE);  
    }  
        
    //accept the incoming connection 
    addrlen = sizeof(address);  
    puts("Esperando conexiones");  
        
    while(TRUE)  
    {  
        //clear the socket set 
        FD_ZERO(&readfds);  
    
        //add master socket to set 
        FD_SET(master_socket, &readfds);  
        max_sd = master_socket;  
            
       
        for ( i = 0 ; i < max_clients ; i++)  
        {  
            // agrego el descriptor de socket
            sd = client_socket[i];  
                
            //if valid socket descriptor then add to read list 
            if(sd > 0)  
                FD_SET( sd , &readfds);  
                
            //highest file descriptor number, need it for the select function 
            if(sd > max_sd)  
                max_sd = sd;  
        }  
    
        //Espera por alguna actividad en los sockets , timeout es NULL por lo que espera indefinidamente 
        activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL);  
      
        if ((activity < 0) && (errno!=EINTR))  
        {  
            printf("select error");  
        }  
            
        //If something happened on the master socket , 
        //then its an incoming connection 
        if (FD_ISSET(master_socket, &readfds))  
        {  
            if ((new_socket = accept(master_socket,(struct sockaddr *)&address, (socklen_t*)&addrlen))<0)  
            {  
                perror("accept");  
                exit(EXIT_FAILURE);  
            }  
            
            //inform user of socket number - used in send and receive commands 
            printf("Nodo Conectado , socket fd is %d , ip is : %s , port : %d\n" , new_socket , inet_ntoa(address.sin_addr) , ntohs(address.sin_port));  
                
            //Añado un nuevo socket al arreglo
            for (i = 0; i < max_clients; i++)  
            {  
                //Si la posicion es vacia
                if( client_socket[i] == 0 )  
                {  
                    client_socket[i] = new_socket;  
                    cantConexiones++;
                    //printf("Añadido a la lista de sockets as %d\n" , i);  
                        
                    break;  
                }  
            }  
        }  
        //Sincronizo el inicio de la generacion del archivo---------
        if(cantConexiones == 2){
			char opcion;
			printf("Arrancar Generador? Y/N\n");
			scanf("%c",&opcion);
			if(opcion=='Y'){ 
			cantConexiones=0;
			//Envio señal de inicio
			send(client_socket[0],"a",4,0);//Nodo 1
			send(client_socket[1],"a",4,0);//Nodo 2
			printf("Generador en funcionamiento\n");
			//llamo al generador local
			generadorLocal();
			}
		}
        //------------------------------------------------------------
         
        //else its some IO operation on some other socket
        for (i = 0; i < max_clients; i++)  
        {  
            sd = client_socket[i];  
                
            if (FD_ISSET( sd , &readfds))  
            {  
                //Check if it was for closing , and also read the 
                //incoming message 
                
                if ((valread = recv( sd , palabrasNodo, sizeof(palabrasNodo),MSG_WAITALL)) == 0)  
                {  
                    //Somebody disconnected , get his details and print 
                    getpeername(sd , (struct sockaddr*)&address , (socklen_t*)&addrlen);  
                    printf("Nodo disconectado , ip %s , port %d \n" ,inet_ntoa(address.sin_addr) , ntohs(address.sin_port));  
                        
                    //Close the socket and mark as 0 in list for reuse 
                    close( sd );  
                    client_socket[i] = 0;  
                }  
                    
                //Echo back the message that came in 
                else
                {  
                    //almaceno las palabras en un archivo
                   // printf("Lei %d bytes\n",valread); //Solo para control de que recibo la cantidad de bytes que tengo que recibir
					escribirArchivo();
                    
                }  
            }  
        }  
    }  
        
    return 0;  
}  

