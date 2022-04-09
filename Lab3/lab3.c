#include<stdio.h>
#include<math.h>
#include<stdlib.h>
#include<unistd.h>
#include<pthread.h>
#include <semaphore.h>
#include"Funciones.h"

sem_t vacio; //Semaforo
sem_t lleno; //Semaforo
int in = 0;
int out = 0;
Particula * buffer; //Buffer que sera usado por los productores/consumidores
int bufferSize; //Tamaño del buffer
pthread_mutex_t mutex; //Se define el mutex a utilizar
pthread_mutex_t mutex2; //Mutex para escritura en el arreglo
float * estructuraPrincipal; //Estructura principal que almacenara los resultados
int estructuraSize; //Tamaño de la estructura principal
int cantidadParticulas; //Cantidad de particulas totales


//Entradas:  Nombre de archivo de entrada
//Funcionamiento: Hebra que leera del archivo de entrada y cargara al buffer las particulas y su posicion de impacto
//Salidas:
void * productor(void *nombreArchivoEntrada) {   
    char * nombre;
    nombre= (char *) nombreArchivoEntrada;
    FILE *archivoBombardeo = leerArchivo(nombre); // Archivo de entrada
    for(int i = 0; i < cantidadParticulas; i++) {
        //Se genera una nueva particula leyendola del archivo de entrada
        int posicion;
        int energiaPotencial;    
        fscanf(archivoBombardeo, "%d", &posicion);
        fscanf(archivoBombardeo, "%d", &energiaPotencial);
        Particula nuevaParticula = crearParticula(posicion, energiaPotencial);
        //Se espera en caso el buffer se encuentre lleno
        sem_wait(&vacio);
        pthread_mutex_lock(&mutex); //Se entra a la seccion critica, entonces se lockea usando mutex
        buffer[in] = nuevaParticula; //Se almacena la particula en el buffer
        in = (in+1)%bufferSize;
        pthread_mutex_unlock(&mutex); //Se sale de la seccion critica, se desbloquea usando mutex
        sem_post(&lleno);
    }
}

//Entradas: Cantidad de particulas a procesar
//Funcionamiento: Hebra que consumira del buffer una cantidad de particular a procesar y efectuara el registro de resultados a una estructura global
//Salidas:
void * consumidor(void *particulas) {   
    int cantidadParticulasAProcesar = *((int *)particulas);
    Particula * arregloParticulas = malloc(sizeof(Particula)*cantidadParticulasAProcesar);
    for(int i = 0; i < cantidadParticulasAProcesar; i++) {
        sem_wait(&lleno);
        //Se bloquea la seccion critica
        pthread_mutex_lock(&mutex);
        Particula particula = buffer[out];
        out = (out+1)%bufferSize;
        arregloParticulas[i] = particula;
        pthread_mutex_unlock(&mutex);
        sem_post(&vacio);
    }
    //Se efectua el bombardeo en la estructura principal
    for(int j = 0; j < cantidadParticulasAProcesar; j++){
        for(int i = 0; i < estructuraSize; i++){
            pthread_mutex_lock(&mutex2); //Se bloquea para aplicar el bombardeo
            estructuraPrincipal[i] = energiaAplicada(estructuraPrincipal[i],i,arregloParticulas[j],estructuraSize);
            pthread_mutex_unlock(&mutex2); //Se desbloquea una vez se termine de aplicar el bombardeo
        }
    }
    free(arregloParticulas);
}



int main(int argc, char **argv){
    //Definicion de variables de entrada a ocupar
    time_t inicio,fin;
    inicio = clock(); //Se empieza a medir la ejecucion del programa
    int estaD = 0;
    int N=-1;
    int cantLineas = -1; //cantidad de lineas archivo de entrada
    int tamanoBuffer = -1;
    int cantHebrasConsumidoras = -1;
    char *nombreEntrada = NULL;
    char *nombreSalida = NULL;
    int c;
    //Ciclo while con switch para validar parametros de entrada
    while ( (c = getopt(argc, argv, "N:c:i:o:b:h:D")) != -1){
        switch (c)
        {
        case 'N': //numero de celdas
            sscanf(optarg, "%d", &N);
            break;

        case 'c': //cantidad de lineas del archivo de entrada
            sscanf(optarg, "%d", &cantLineas);
            break;

        case 'i': //archivo de bombardeo
            nombreEntrada = optarg;
            break;

        case 'o': //archivo de salida
            nombreSalida = optarg;
            break;
        
        case 'b': //tamaño del buffer de la hebra productora
            sscanf(optarg, "%d", &tamanoBuffer);
            break;

        case 'h': //numero de hebras consumidoras
            sscanf(optarg, "%d", &cantHebrasConsumidoras);
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
    //Verificaciones
    if(N <= 0){
        printf("N invalido\n");
        return 0;
    }
    if(tamanoBuffer<=0){
        printf("Tamano buffer invalido\n");
        return 0;
    }
    if(cantHebrasConsumidoras <= 0){
        printf("Cantidad hebras consumidoras es invalida\n");
        return 0;
    }
    if(cantLineas <= 0){
        printf("Cantidad lineas invalidas\n");
        return 0;
    }
    if(cantHebrasConsumidoras > cantLineas){
        printf("Cantidad hebras consumidoras no puede ser mayor a la cantidad de lineas, para no generar hebras viciosas\n");
        return 0;
    }
    if(tamanoBuffer > cantLineas){
        printf("Cantidad hebras consumidoras no puede ser mayor a la cantidad de lineas, para no generar hebras viciosas\n");
        return 0;
    }
    
    // Se calcula la cantidad de particulas a procesar por cada hebra
    //Se crea un arreglo que almacenara la cantidad de particulas a procesar por hebra
    int cantParticulasProcesar = cantLineas / cantHebrasConsumidoras;
    int lineasFaltantes =  cantLineas - (cantParticulasProcesar*cantHebrasConsumidoras);
    int * arregloCantParticulasProcesar = malloc(sizeof(int)*cantHebrasConsumidoras);
    for(int i = 0; i < cantHebrasConsumidoras ; i++){
        arregloCantParticulasProcesar[i] = cantParticulasProcesar;
    }
    //Se asignan las lineas faltantes
    for(int i=0;i<lineasFaltantes;i++){
        arregloCantParticulasProcesar[i]=arregloCantParticulasProcesar[i]+1;
    }
    //Se fija la cantidad de particulas totales
    cantidadParticulas = cantLineas;

    //Se inicializa el buffer asignandole memoria segun su tamaño
    bufferSize = tamanoBuffer;
    buffer = malloc(sizeof(Particula)* bufferSize);

    //Se inicializa la estructura principal asignandole memoria segun su tamaño
    estructuraSize = N;
    estructuraPrincipal = malloc(sizeof(float)*estructuraSize);

    //Se inicializa el mutex
    pthread_mutex_init(&mutex, NULL);
    pthread_mutex_init(&mutex2, NULL); //test

    //Se inicializan los semaforos que ocupara el buffer
    sem_init(&vacio,0,bufferSize);
    sem_init(&lleno,0,0);

    pthread_t hebraProductora; //Se inicializa la hebra productora
    pthread_t hebraConsumidora[cantHebrasConsumidoras]; //Se inicializan la hebras consumidoras

    pthread_create(&hebraProductora, NULL, (void *)productor, (void *)nombreEntrada); //Se crea la hebra productora

    for(int i = 0; i < cantHebrasConsumidoras; i++){
        pthread_create(&hebraConsumidora[i],NULL, (void *)consumidor,(void *)&arregloCantParticulasProcesar[i]); //Se crea una hebra consumidora
    }

    //Se espera a que terminen las hebras productoras y consumidoras
    pthread_join(hebraProductora, NULL);

    for(int i = 0; i < cantHebrasConsumidoras; i++){
        pthread_join(hebraConsumidora[i], NULL);
    }

    //Se destruye el mutex y los semaforos ocupados
    pthread_mutex_destroy(&mutex);
    pthread_mutex_destroy(&mutex2);
    sem_destroy(&vacio);
    sem_destroy(&lleno);

    //Se pregunta si esta el Flag D
    if(estaD){
        int indiceMaximo = maximoArreglo(estructuraPrincipal,estructuraSize);
        niceprint(estructuraSize, estructuraPrincipal, estructuraPrincipal[indiceMaximo]);
    }
    //Se escribe la salida
    escribirSalida(nombreSalida,estructuraPrincipal,estructuraSize);
   
    free(estructuraPrincipal);
    return 0;
}
