#include<stdio.h>
#include<math.h>
#include<stdlib.h>
#include<unistd.h>
#include<time.h>
#include"Funciones.h"



int main(int argc, char **argv){
    //Definicion de variables de entrada a ocupar
    time_t inicio,fin;
    inicio = clock(); //Se empieza a medir la ejecucion del programa
    int estaD = 0;
    int N;
    char *nombreEntrada = NULL;
    char *nombreSalida = NULL;
    int c;
    //Ciclo while con switch para validar parametros de entrada
    while ( (c = getopt(argc, argv, "N:i:o:D")) != -1){
        switch (c)
        {
        case 'N': //numero de celdas
            sscanf(optarg, "%d", &N);
            break;

        case 'i':
            nombreEntrada = optarg;
            break;

        case 'o':
            nombreSalida = optarg;
            break;

        case 'D': //opcional
            estaD = 1;
            break;
        case '?':
            if(optopt == 'o'){
                fprintf (stderr, "Opcion -%c requiere un argumento.\n", optopt);
                exit(0);
            }
            if(optopt == 'i'){
                fprintf (stderr, "Opcion -%c requiere un argumento.\n", optopt);
                exit(0);
            }
        default: //En caso que no se haya ingresado alguna flag obligatoria, se aborta la ejecucion
            abort();
        }
    }
    int largoBombardeos = 0; // largo bombardeos
    float *material = malloc(sizeof(float) * N);
    FILE *archivoBombardeo = leerArchivo(nombreEntrada); // Archivo de entrada
    Particula *bombardeo = archivoParticulas(archivoBombardeo, &largoBombardeos);
    //Se inicializa el arreglo
    for (int i = 0; i < N; i++){
        material[i] = 0;
    }
    //Se aplica el bombardeo.
    for(int j = 0; j < largoBombardeos; j++){
        for(int i = 0; i < N; i++){
            material[i] = energiaAplicada(material[i],i,bombardeo[j],N);

        }
    }
    if(estaD){
        int indiceMaximo = maximoArreglo(material,N);
        niceprint(N, material, material[indiceMaximo]);
    }
    escribirSalida(nombreSalida,material,N);
    free(bombardeo);
    free(material);
    fin = clock(); //Se finaliza la medicion de tiempo de ejecucion
    double tiempoTotal = (double)(fin - inicio) / CLOCKS_PER_SEC; //Se calcula el tiempo de ejecucion
    printf("Tiempo de ejecucion: %f\n",tiempoTotal);
    return 0;
}
