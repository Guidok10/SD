#include <stdlib.h>
#include <stdio.h>
#include <string.h>


struct Encabezado{
	long unsigned cant_palabras;
	long unsigned tama;
	int ID;
};
typedef struct Encabezado enca_t;

//Ejecuta quick por unica vez y retorna la posicion del pivot
int quicksortPivot(char **arre,int first,int last){
    int pivot,j,i; 	// Índices
    char* temp;		// Puntero Auxiliar

     if(first<last){
         pivot=first;
         i=first;
         j=last;

         while(i<j){
             while(strcmp(arre[i],arre[pivot]) <= 0 && i<last)
                 i++;
             while(strcmp(arre[j],arre[pivot]) > 0)
                 j--;
             if(i<j){
                 temp=arre[i];
                  arre[i]=arre[j];
                  arre[j]=temp;
             }
	              
	}

        temp=arre[pivot];
        arre[pivot]=arre[j];
        arre[j]=temp;
    }
    return j;
}

void quicksort(char **arre,int first,int last){
    int pivot,j,i; 	// Índices
    char* temp = (char*) malloc(11*sizeof(char));		// Puntero Auxiliar

     if(first<last){
         pivot=first;
         i=first;
         j=last;

         while(i<j){

             while(strcmp(arre[i],arre[pivot]) <= 0 && i<last)
                 i++;
             while(strcmp(arre[j],arre[pivot]) > 0)
                 j--;
             if(i<j){
                strncpy(temp,arre[i],11);
                strncpy(arre[i],arre[j],11);
                strncpy(arre[j],temp,11);
             }
	              
	}

         strncpy(temp,arre[pivot],11);
         strncpy(arre[pivot],arre[j],11);
         strncpy(arre[j],temp,11);
         quicksort(arre,first,j-1);
         quicksort(arre,j+1,last);

    }
}
