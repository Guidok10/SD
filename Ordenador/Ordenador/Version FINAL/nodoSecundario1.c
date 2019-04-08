//Estructura
//Quick
#include "Auxiliares.c"

/*Cliente tcp*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <time.h>
#include <sys/socket.h>
#include <strings.h>
#include <netdb.h>
#include <netinet/in.h>
/*libreria para la parte de hilos*/
#include <pthread.h>

#define PORT1 13131
#define BACKLOG 2 /* El número de conexiones permitidas */

typedef struct data{
    char** cadena;
    int posini,posfin;
}parametro;


void* funcionOrdenar(void *args){
  
    parametro* par=(parametro*)args;
    quicksort((char**)par->cadena,par->posini,par->posfin);       
}

int main(int argc, char *argv[])
{

    int puerto;

    if (argc !=3) {
        printf("Uso: %s <IP_server> <Puerto>\n",argv[0]);
        exit(-1);
    }
    puerto =atoi(argv[2]);
    //puerto = PORT1;

    //Descriptor
    int fd; 

    size_t numbytes;
    struct hostent *he; // estructura que recibirá información sobre el nodo remoto 
    struct sockaddr_in server; // información sobre la dirección del servidor 
    
    //hilos
    pthread_t hilo1,hilo2;


    int seguir = 1;
    while(seguir){        
        seguir = 1;

        if ((he=gethostbyname(argv[1]))==NULL){
            printf("gethostbyname() error\n");
            exit(-1);
        }

        if ((fd=socket(AF_INET, SOCK_STREAM, IPPROTO_TCP))==-1){
            printf("socket() error\n");
            exit(-1);
        }

        server.sin_family = AF_INET;
        server.sin_port = htons(puerto);
        server.sin_addr = *((struct in_addr *)he->h_addr); //he->h_addr pasa la información de ``*he'' a "h_addr"
        bzero(&(server.sin_zero),8);

       if(connect(fd, (struct sockaddr *)&server,sizeof(struct sockaddr))==-1){
          printf("connect() error\n");
          exit(-1);
       }
    
        // Recibo el enabezado
        enca_t *enc = (enca_t *) malloc(sizeof(struct Encabezado));


        if ((numbytes=recv(fd,enc,sizeof(struct Encabezado),0)) < 0){
            printf("Error en recv() \n");
            exit(-1);
        }
        else
        {
            if(numbytes>0){
               //Envio del mensaje tipo 7 (ACK)
                int tipomsj[1];
                tipomsj[0] = 7;

                send(fd,tipomsj,sizeof(tipomsj),0);

                //Extraigo datos del encabezado 
                long unsigned cant_palabras = enc->cant_palabras; 
                long unsigned tama = enc->tama; 
                char *msg;
                msg = malloc(sizeof(char)*10*cant_palabras);

                if ((numbytes=recv(fd,msg,tama,MSG_WAITALL)) == -1){
                    printf("Error en recv() \n");
                    exit(-1);
                }

                //Armo el arreglo de char*
                char **arreglo = malloc(11 * sizeof(char*) * cant_palabras);
                for(int i = 0; i < cant_palabras; i++){
                    arreglo[i] = malloc(10*sizeof(char));
                    strncpy(arreglo[i],msg+10*i,10);
                    arreglo[i][10]='\0';   
                }

                printf("cant palabras recibidas: %lu \n",cant_palabras);
                  
                /*Ahora cargamos las dos mitades al struct "data" (p1 y p2)*/
                parametro p1,p2;

                int pospivot = quicksortPivot(arreglo,0,cant_palabras-1);

                p1.cadena=arreglo;
                p1.posini=0;
                p1.posfin=pospivot - 1;

                p2.cadena=arreglo;
                p2.posini=pospivot+1;
                p2.posfin=cant_palabras - 1;

                  
                pthread_create(&hilo1,NULL,funcionOrdenar, (void*)&p1);
                pthread_create(&hilo2,NULL,funcionOrdenar,(void*)&p2);
                   
                /*Esperamos a que los hilos terminen, si no hacemos esto no podemos ver la ejecucion del hilo*/
                   
                pthread_join(hilo1,NULL);
                pthread_join(hilo2,NULL);

                //SIN HILOS
                /*
                quicksort(arreglo,0,cant_palabras-1);*/


                //Cargo el arreglo ordenado para enviar
                memset(msg,0,tama); //Limpiar el arreglo
                for(int i = 0; i < cant_palabras ; i++){
                    strncpy(msg+10*i,arreglo[i],10);     
                }
                msg[sizeof(char)*10*cant_palabras]='\0';

                //Envio del arreglo ordenado
                send(fd,msg,tama,0);   
            }
        }   
    }
    close(fd);
    return(0);
}
