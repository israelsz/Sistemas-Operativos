#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include"Funciones.h"
#include<math.h>

int main(int argc, char **argv){
    
    //Se inicializan variables que seran leidas por el pipe
    char nombreEntrada[100];
    int Linicio;
    int Lfinal;
    int N;
    int cantidadParticulasTotales;

    //Se leen y almacenan los datos recibidos del padre
    read(STDIN_FILENO, nombreEntrada, sizeof(char)*100);
    read(STDIN_FILENO, &Linicio, sizeof(int));
    read(STDIN_FILENO, &Lfinal, sizeof(int));
    read(STDIN_FILENO, &N, sizeof(int));
    read(STDIN_FILENO, &cantidadParticulasTotales, sizeof(int));
    
    //Se aplica el bombardeo con los parametros recibidos
    float* arregloParcial = aplicarEnergia(Linicio,Lfinal,N,nombreEntrada,cantidadParticulasTotales);
    //Se envia el arreglo parcial de vuelta al padre a través del pipe
    for(int i=0;i<N;i++){
        write(STDOUT_FILENO,&arregloParcial[i],sizeof(float));
    }
    return 0;
}