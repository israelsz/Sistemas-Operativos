#include <stdio.h>
#include <stdlib.h>
#include "Funciones.h"
#include <math.h>



//Entradas: Dos enteros: la posicion j y energia potencial
//Funcionamiento: Crea una nueva Particula segun la struct definida
//Salidas: Una nueva variable del tipo Particula

Particula crearParticula(int j, int Ep){
    Particula P;
    P.j = j;
    P.Ep = Ep;
    return P;
}

//Entradas: Una Particula
//Funcionamiento: Muestra por pantalla la posicion y la energia potencial de la particula
//Salidas:
void mostrarParticula(Particula p){
    printf("j:%d Ep:%d \n",p.j,p.Ep);
}

//Entradas: Un archivo, un entero
//Funcionamiento: Crea un arreglo que almacena todas las particulas del archivo, ademas calcula el largo de este arreglo por referencia
//Salidas: Arreglo de Particulas que contiene todas las particulas del archivo de entrada
Particula * archivoParticulas(FILE *f, int *largoBombardeos){
    int nParticulas = 0;
    int j,Ep;
    fscanf(f, "%d", &nParticulas);
    *largoBombardeos = nParticulas;
    Particula * bombardeos = (Particula *) malloc(sizeof(Particula) * nParticulas);
    for(int i = 0; i < nParticulas; i++){
        fscanf(f, "%d %d\n", &j, &Ep);
        bombardeos[i] = crearParticula(j,Ep);
    }
    fclose(f);
    return bombardeos;

}

//Entradas: Un arreglo de flotantes, entero que representa el largo del arreglo
//Funcionamiento: Muestra por pantalla los elementos del arreglo
//Salidas:
void mostrarArregloFloat(float *A,int largo){
    for(int i = 0; i < largo; i++){
        printf("%i :%.4f \n", i, A[i]);
    }
    printf("\n");
}

//Entradas: flotante, entero, particula, entero: energia que posee el arreglo de energia, posicion a bombardear, particula que bombardea, largo de arreglo a bombardear
//Funcionamiento: aplica la formula de energia respetando el umbral minimo
//Salidas: resultado formula de energia
float energiaAplicada(float Ei,int i,Particula p, int N){
    int j = p.j;
    int Ep = p.Ep;
    float MIN_ENERGY = pow(10,-3);
    float energia = Ei + ((pow(10,3)) * Ep) / (N * (sqrt(fabs(j - i) + 1)));
    if(energia <= MIN_ENERGY){
        return 0;
    }
    return energia;
}


//Entradas: puntero a caracter: nombre del archivo a leer
//Funcionamiento: intenta abrir el archivo, si pudo abrirlo retorna el archivo, si no puede retorna 0
//Salidas: archivo: archivo leido
FILE * leerArchivo(char* nombreArchivo){
    //Se inicializa archivo como null
    FILE* archivo = NULL;
    archivo = fopen(nombreArchivo, "r"); //Lee el archivo
    if (archivo == NULL){ //Si el archivo no existe.
        printf("No se pudo abrir archivo\n");
        exit(0);
    } else {
        return(archivo);
    }
}

extern void niceprint(int N, float *Energy, float Maximum); //Niceprint

//Entradas: Arreglo de la estructura con las energias, largo del arreglo
//Funcionamiento: Encuentra la posicion del arreglo donde esta el elemento con mayor energia
//Salidas: numero entero que representa el indice donde se encuentra el elemento con mas energia
int encontrarIndiceMayor(float* energy, int N){
    float mayorEnergy = energy[0];
    int indiceMayor = 0;
    for(int i = 0; i < N; i++){
        //Si encuentra un indice con mayor energia
        if(energy[i] > mayorEnergy){
            mayorEnergy = energy[i];
            indiceMayor = i;
        }
    }
    return indiceMayor;
}

//Entradas: puntero a caracter, puntero a flotante, entero: nombre del archivo de salida, arreglo de energia, largo del arreglo de energia
//Funcionamiento: encuentra la posicion con mas energia y escribe el archivo de salida 
//Salidas: vacio
void escribirSalida(char *nombreSalida, float *Energy, int N){
    //Se busca el indice que tiene la mayor energia
    int indiceMayor = encontrarIndiceMayor(Energy, N);
    //Se crea el archivo de salida
    FILE * salida; //Se define un archivo salida
    salida = fopen(nombreSalida, "w"); //Se abre el archivo salida en modo de escritura y se le asigna el nombre salida.txt
    float mayorEnergy=Energy[indiceMayor];
    fprintf(salida,"%d %.6f\n", indiceMayor, mayorEnergy);
    //Se escribe el archivo de salida
    for(int i =0; i < N; i++){
        fprintf(salida,"%d %.6f\n", i, Energy[i]);
    }
    fclose(salida);
}

//Entradas: entero: linea de inicio del archivo, entero: linea final del archivo, entero: numero de celdas, puntero a caracter: nombre del archivo de entrada, entero: cantidad de bombardeos que posee el archivo
//Funcionamiento: lee el archivo de entrada y aplica los bombardeos que correspondan desde la linea de inicio hasta la linea final ingresadas, almacenando sus resultados en un arreglo del mismo tamaño que la estructura principal
//Salidas: arreglo de flotantes con los resultados del bombardeo
float *aplicarEnergia(int Linicio, int Lfinal, int N, char *nombreArchivo, int cantidadParticulasTotales){
    FILE * f = leerArchivo(nombreArchivo); // Se abre el archivo con bombardeos
    //Se inicializa el arreglo que acumula energia
    float *energia = malloc(sizeof(float)*N);
    //Se setea en 0 el arreglo
    for(int i=0;i<N;i++){
        energia[i] = 0;
    }
    int j,Ep; // Variables auxiliares para almacenar los parametros de bombardeo del archivo
    Particula * bombardeos = (Particula *) malloc(sizeof(Particula) * cantidadParticulasTotales); //Arreglo para almacenar los bombardeos
    //Se lee el archivo y se almacenan los bombardeos leidos
    for(int i = 0; i < cantidadParticulasTotales; i++){
        fscanf(f, "%d %d\n", &j, &Ep);
        bombardeos[i] = crearParticula(j,Ep);
    }
    fclose(f); //Se cierra el archivo
    //Se aplica los bombardeos leidos previamente
    for(int j = Linicio; j < Lfinal; j++){
        for(int i = 0; i < N; i++){
            energia[i] = energiaAplicada(energia[i],i,bombardeos[j],N);
        }
    }
    free(bombardeos); // Se libera la memoria del arreglo de bombardeos
    return energia; //Retorno del arreglo de energia
};
