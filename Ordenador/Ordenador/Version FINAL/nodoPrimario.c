/*
	Ejercicio 2 - Nodo primario - Proyecto N°2 Sistemas Distribuidos 
	Equipo AZUL
*/

//Quick
#include "Auxiliares.c"
//Estructura   
#define CANT_PALABRAS 128000

/*Sevidor TCP*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <time.h>

#define BACKLOG 2 /* El número de conexiones permitidas */
#define PALABRASPORRENGLON 20

#define PORT1 13131 // El Puerto Abierto del nodo remoto 
#define PORT2 13141

//Variables del manejo del archivo
FILE *arch_dest1;
FILE *arch_dest2;
FILE *arch_origen;


void escribir_arch_dest(FILE* arch_dest, char *msg, int tam, int ns ){

	int i = 0;

	while(i<tam){
		if((i%10) == 0){ //formo una palabra
			if(i==0 && ns==1)
				fprintf (arch_dest, "%c",msg[i]);
			else
				fprintf (arch_dest, " %c",msg[i]);
		}
		else	
			fprintf (arch_dest, "%c",msg[i]);
		i++;
	}
}

int main(int argc, char *argv[]){

	if (argc !=3) {
      	printf("Uso: %s <puerto> <Nombre_de_archivo>\n",argv[0]);
      	exit(-1);
   	}

   	int puerto = atoi(argv[1]);

   	char nombreArch[strlen(argv[2])];
   	strncpy(nombreArch,argv[2],strlen(argv[2]));

	int fd1, fd2; 
	int numbytes;	
	struct sockaddr_in server; /* para la información de la dirección del servidor */
	struct sockaddr_in client;/* para la información de la dirección del cliente */
	int sin_size;

	int palabrasSeg = 0;
			
	//Variables del protocolo
	enca_t *enc1;
	enca_t *enc2;
	long unsigned tama1;
	long unsigned tama2;
	pid_t hijo,hijo2;
	int status1, status2;
	int rta[1];

	//Variables de los arreglos de char
	char *palabras;
	char **arreglo;
	char *msg1;
	char *msg2;
	long unsigned pospivot;

	//Variables para tomar el tiempo de ejecución
	struct timeval ini,fin;

	
	//ALOCACION DE BUFFERS
	palabras = malloc(sizeof(char)*11*CANT_PALABRAS);
	arreglo = malloc(11 * sizeof(char*) * CANT_PALABRAS );

	int seguir = 1;
	int control = 1;
	int op;


	/* A continuación la llamada a socket() */
	if ((fd1=socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1 ) {
		printf("error en socket()\n");
		exit(1);
	}

	server.sin_family = AF_INET;
	server.sin_port = htons(puerto);
	server.sin_addr.s_addr = INADDR_ANY; // INADDR_ANY coloca nuestra dirección IP automáticamente 
	bzero(&(server.sin_zero),8); // escribimos ceros en el reto de la estructura


	/* A continuación la llamada a bind() */
	if(bind(fd1,(struct sockaddr*)&server,sizeof(struct sockaddr))==-1) {
		printf("error en bind() \n");
		exit(1);
	}

	if(listen(fd1,BACKLOG) == -1) {  /* llamada a listen() */
		printf("error en listen()\n");
		exit(1);
	}

	while(seguir) {
		//seguir = control = 0;

		control = 1;
		while (control){
			printf("Elija la opcion que desee: \n");
			printf("	1- ordenar archivo \n");
			printf("	2- ordenar otro archivo \n");
			printf("	3- salir \n");
			
			scanf("%i",&op);
			if (op>0 && op<4)
				control=0;
			else{
                printf("Ingrese una opcion valida\n");
                fflush(stdin);
            }
            if (op == 3)
            	seguir = 0;

            if (op ==2){
            	printf("Ingrese el nombre del nuevo archivo: \n");
            	scanf("%s",nombreArch);
            }

		}

		//if (seguir == 0){
		if (seguir){	
			//Reseteo de archivo para hijo 1 
			arch_dest1 = fopen("ordenado","w");
			if(arch_dest1 == 0)
				perror("Apertura de archivo salida - testing ");
			fclose(arch_dest1);

			//Reseteo de archivo para hijo 2 
			arch_dest2 = fopen("ordenado2","w");
			if(arch_dest2 == 0)
				perror("Apertura de archivo salida - testing ");
			fclose(arch_dest2);


			//ABRIR LOS ARCHIVOS

			arch_dest1 = fopen("ordenado","w");
			if(arch_dest1 == 0) perror("Apertura de archivo destino - testing ");

			arch_dest2 = fopen("ordenado2","w");
			if(arch_dest2 == 0) perror("Apertura de archivo destino - testing ");

			//arch_origen  = fopen("generado_testing","r");
			printf("Procedo a abrir %s: \n",nombreArch);
			arch_origen  = fopen(nombreArch,"r");
			if(arch_origen  == 0) perror("Apertura de archivo origen fallida ");

			// CARGAR PALABRAS AL BUFFER
			//fgets(palabras,11*CANT_PALABRAS,arch_origen);

			char ch;
			int index = 0;
			ch = getc ( arch_origen );
			while ( ch != EOF ) {
        		if ( ch != '\n'){
            		palabras[index++] = ch;
        		}
        		else {
           			index++;
        		}
       			ch = getc ( arch_origen );
    		}

			fclose(arch_origen);	   


			//CARGAR EL ARREGLO DE PUNTEROS A char*
			for(int i = 0; i < CANT_PALABRAS ; i++){
				arreglo[i] = malloc(10*sizeof(char));
				strncpy(arreglo[i],palabras+11*i,10);   
			}


			//Comienzo de ejecucion
			printf("Primer accept, empiezo a tomar tiempo\n");
			
			gettimeofday(&ini,NULL); //inicio
		
			// EJECUTAR ORDENAMIENTO DEL PIVOT
			pospivot = quicksortPivot(arreglo,0,CANT_PALABRAS-1);
			
			//CARGAR EL ARREGLO a enviar 1
			msg1 = malloc(sizeof(char)*10*pospivot+1);
			for(int i = 0; i < pospivot ; i++){
				strncpy(msg1+10*i,arreglo[i],10);     
			}

			msg1[sizeof(char)*10*pospivot]='\0';
		
			//CARGAR EL ARREGLO a enviar 2
			palabrasSeg = CANT_PALABRAS - pospivot - 1;
			msg2 = malloc(sizeof(char)*10*palabrasSeg+1);

			int j = 0;
			for(int i = pospivot+1; i < CANT_PALABRAS ; i++){
				strncpy(msg2+10*j,arreglo[i],10); 
				j++;     
			}

			msg2[sizeof(char)*10*palabrasSeg]='\0';
					
			tama1 = sizeof(char)*10*pospivot;
			tama2 = sizeof(char)*10*palabrasSeg;
			
			/*Fin del codigo para generar los dos arreglos*/ 

			//Armado de los struct encabezado
			enc1 = (enca_t *) malloc(sizeof(struct Encabezado));
			enc1->cant_palabras = pospivot;
			enc1->tama = tama1;
			enc1->ID = 1;

			enc2 = (enca_t *) malloc(sizeof(struct Encabezado));
			enc2->cant_palabras = palabrasSeg;
			enc2->tama = tama2;
			enc2->ID = 2;
		



			/* A continuación la primer llamada a accept() */
			if ((fd2 = accept(fd1,(struct sockaddr *)&client,&sin_size))==-1) {
				printf("error en accept1()\n");
				exit(1);
			}
			//FORK
			hijo = fork();

			if (hijo==0){

				printf("HIJO1 - Se obtuvo una conexión desde %s\n", inet_ntoa(client.sin_addr) );

				//Envio el encabezado
				send(fd2,enc1,sizeof(struct Encabezado),0);

				//Espero el ACK
				rta[0] = 0;	
				if ((numbytes=recv(fd2,rta,sizeof(rta),0)) == -1){
					printf("Error en recv() \n");
					exit(-1);
				}
				if (rta[0] == 7){ //Mensaje de tipo 7 = recepcion correcta del encabezado (ACK)
					printf("obtuvo su encabezado...envio del mensaje\n");
					//Envio del mensaje
					send(fd2,msg1,tama1,0);
				}

				//A continuacion se queda esperando a que llegue el arreglo ya ordenado
				memset(msg1,0,tama1); //Limpiar el arreglo
				if ((numbytes=recv(fd2,msg1,tama1,MSG_WAITALL)) == -1){
					printf("Error en recv() \n");
					exit(-1);
				}

				escribir_arch_dest(arch_dest1,msg1,tama1,1); 

				close(fd2);				
				exit (0);
			}
			else{
				/* A continuación la segunda llamada a accept() */
				if ((fd2 = accept(fd1,(struct sockaddr *)&client,&sin_size))==-1) {
					printf("error en accept2()\n");
					exit(1);
				}

				hijo2 = fork();
				if (hijo2==0){

					printf("HIJO 2 - Se obtuvo una conexión desde %s\n", inet_ntoa(client.sin_addr) );

					//Envio el encabezado
					send(fd2,enc2,sizeof(struct Encabezado),0);

					//Espero el ACK
					rta[0] = 0;	
					if ((numbytes=recv(fd2,rta,sizeof(rta),0)) == -1){
						printf("Error en recv() \n");
						exit(-1);
					}
					if (rta[0] == 7){ //Mensaje de tipo 7 = recepcion correcta del encabezado (ACK)
						printf("obtuvo su encabezado...envio del mensaje\n");
						//Envio del mensaje
						send(fd2,msg2,tama2,0);
					}

					//A continuacion se queda esperando a que llegue el arreglo ya ordenado
					memset(msg2,0,tama2); //Limpiar el arreglo
					if ((numbytes=recv(fd2,msg2,tama2,MSG_WAITALL)) == -1){
						printf("Error en recv() \n");
						exit(-1);
					}
					
					escribir_arch_dest(arch_dest2,arreglo[pospivot],10,2);
					escribir_arch_dest(arch_dest2,msg2,tama2,2); 

					fclose(arch_dest2);

					close(fd2);
					exit(0);
				}	
				else{

					waitpid(hijo,&status1,0);
					waitpid(hijo2,&status2,0);

					//Una vez que está el archivo ordenado con las 128000 palabras, corto el tiempo de ejecucion
					gettimeofday(&fin,NULL); //fin 

					arch_dest2 = fopen("ordenado2","r");
					if(arch_dest2 == 0) 
						perror("Apertura de archivo destino - testing ");

					char c = fgetc(arch_dest2);
					while(c!= EOF){
						fputc(c, arch_dest1);
       					c = fgetc(arch_dest2);
					}

					fclose(arch_dest1);
					fclose(arch_dest2);

					uint64_t diferencia = 1000000 * (fin.tv_sec - ini.tv_sec) + (fin.tv_usec - ini.tv_usec);

    				printf("Tiempo de ejecucion: %ld micros\n", diferencia);
				}			
			}
		}

	}

	close(fd1);
	close(fd2);	
	return(0);
}

