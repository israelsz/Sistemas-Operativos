#include<stdio.h>
#include<math.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>
#include<sys/types.h>
#include"Funciones.h"


//Se definen lecturas y escriturar para pipes
#define LECTURA 0
#define ESCRITURA 1


int main(int argc, char **argv){
    //Definicion de variables de entrada a ocupar
    int estaD = 0;
    int N;
    int n=0; //Cantidad de particulas a procesar
    char *nombreEntrada = NULL;
    char *nombreSalida = NULL;
    int cantLineas=0; //cantidad de lineas archivo de entrada
    int c;
    int p; //numero de procesos
    //Ciclo while con switch para validar parametros de entrada
    while ( (c = getopt(argc, argv, "N:p:c:i:o:D")) != -1){
        switch (c)
        {
        case 'N': //numero de celdas
            sscanf(optarg, "%d", &N);
            break;
        case 'p': //Cantida de procesos
            sscanf(optarg, "%d", &p);
            break;
        case 'c': //Cantidad de lineas del archivo de entrada
            sscanf(optarg, "%d", &cantLineas);
            break;
        case 'i': //archivo de bombardeo
            nombreEntrada = optarg;
            break;

        case 'o': //archivo de salida
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
    //Verificar que la cantidad de celdas sea valida
    if (N <= 0){
        printf("Debe ingresar al menos una celda.\n");
        return 0;
    }
    //Verificar si la cantidad de procesos es valida
    if(p <= 0){
        printf("Debe de ingresar al menos 1 proceso para la ejecucion del programa.\n");
        return 0;
    }
    //Se verifica que el archivo de entrada exista
    FILE *f=fopen(nombreEntrada,"r");
    if(f == NULL){ //Si no se pudo abrir
        printf("El archivo de entrada no existe.\n");
        return 0;
    }
    fclose(f); //Se cierra el archivo de entrada
    //Verificar que la cantidad de lineas sea valida
    if(cantLineas <= 0){
        printf("El numero minimo de cantidad de lineas a procesar es 1\n");
        return 0;
    }

    if(p > cantLineas){
        printf("No pueden haber más procesos que cantidad de particulas.\n");
        return 0;
    }

    // Se calcula la cantidad de particulas a procesar por cada proceso hijo
    int cantParticulasProcesar = cantLineas / p; //Cantidad_Particulas / Cantidad procesos

    //Se crea un arreglo que almacenara la cantidad de particulas a procesar por hijo
    int * arregloCantParticulasProcesar = malloc(sizeof(int)*p+1);
    //Si la cantidad de particulas a procesar es par, cada proceso procesara la misma cantidad
    if(cantLineas%p == 0){
        for(int i = 0; i < p ; i++){
            arregloCantParticulasProcesar[i] = cantParticulasProcesar;
        }
        //Si la cantidad de particulas a procesar es impar, el ultimo proceso procesa 1 particula mas de lo normal
    } else {
        for(int i = 0; i < p-1 ; i++){
            arregloCantParticulasProcesar[i] = cantParticulasProcesar;
        }
        arregloCantParticulasProcesar[p-1] = cantParticulasProcesar+1;
    }

    //Se fija la linea inicial a procesar del archivo de entrada
    int lineaInicial = 0;
    int lineaFinal = 0;
    int posicionArregloProceso = 0;

    int lineaInicialCorrespondiente;
    int lineaFinalCorrespondiente;
    float buffer;

    float * estructuraFinal = malloc(sizeof(float)*N);
    for(int i=0;i<N;i++){
        estructuraFinal[i]=0;
    }

    //Se crean los n pipes que se usaran para comunicar
    int **arregloPipesEscritura = (int**)malloc(sizeof(int *)*p); //p = procesos
    int **arregloPipesLectura = (int**)malloc(sizeof(int *)*p); //p = procesos
    for (int i = 0; i < p; i++){
        arregloPipesEscritura[i]= (int*)malloc(sizeof(int)*2); //se guarda para fd[0],fd[1]
        arregloPipesLectura[i]= (int*)malloc(sizeof(int)*2); //se guarda para fd[0],fd[1]
        pipe(arregloPipesEscritura[i]);
        pipe(arregloPipesLectura[i]);
    }

    pid_t pid; //Se inicializa la variable pid
    int status; // estatus de los hijos
     
    //Se crean p hijos de acuerdo a la entrada ingresada
    for(int i=0;i<p;i++){
        //Se asigna de donde hasta donde este hijo debera procesar del archivo de entrada
        lineaInicialCorrespondiente = lineaInicial;
        lineaFinalCorrespondiente = lineaInicial + arregloCantParticulasProcesar[posicionArregloProceso];

        lineaInicial = lineaFinalCorrespondiente;
        posicionArregloProceso = posicionArregloProceso + 1;

        //Se crea el proceso hijo

        pid = fork();

        if (pid == 0) {
            //Si es un hijo se realizara lo siguiente:

            close(arregloPipesEscritura[i][LECTURA]); //Se cierra el lado de Lectura, ya que no leera
            dup2(arregloPipesEscritura[i][ESCRITURA], STDOUT_FILENO); //Se genera una copia del pipe al stdout
            close(arregloPipesEscritura[i][ESCRITURA]);

            close(arregloPipesLectura[i][ESCRITURA]); //Se cierra el lado de Escritura, ya que no escribira
            dup2(arregloPipesLectura[i][LECTURA], STDIN_FILENO); //Se genera una copia del pipe al stdout
            close(arregloPipesLectura[i][LECTURA]);
            
            execl("bomb", "bomb", NULL);
            
            //En caso de error
            perror("execl fallo");
            exit(EXIT_FAILURE);
        }
        else{
            //Si es el padre realiza lo siguiente:

            close(arregloPipesLectura[i][LECTURA]); //Se cierra el de lectura, ya que este pipe sera usado para enviar datos

            //Se envian los parametros necesarios para que sean leidos por los procesos hijos
            write(arregloPipesLectura[i][ESCRITURA], nombreEntrada, sizeof(char)*100);
            write(arregloPipesLectura[i][ESCRITURA], &lineaInicialCorrespondiente, sizeof(int));
            write(arregloPipesLectura[i][ESCRITURA], &lineaFinalCorrespondiente, sizeof(int));
            write(arregloPipesLectura[i][ESCRITURA], &N, sizeof(int));
            write(arregloPipesLectura[i][ESCRITURA], &cantLineas, sizeof(int));
            
            //Espera a los hijos
            waitpid(pid, &status,0);

            close(arregloPipesEscritura[i][ESCRITURA]); //Se cierra su lado de lectura para evitar errores con los procesos hijos
            //Se leen los resultados del hijo y se acumula al resultado de el arreglo parcial en el arreglo principal
            for(int k=0;k<N;k++){
                read(arregloPipesEscritura[i][LECTURA], &buffer, sizeof(float));
                estructuraFinal[k] = estructuraFinal[k] + buffer;
            }
            close(arregloPipesEscritura[i][LECTURA]);
        }
    }
     
    //Si esta el flag D
    if(estaD){
        int indiceMayor = encontrarIndiceMayor(estructuraFinal,N); // Se calculo el indice del mayor
        niceprint(N,estructuraFinal,estructuraFinal[indiceMayor]); //Se llama a niceprint
    }
    for (int i=0;i<p;i++){ //Se libera la memoria de los pipes
        free(arregloPipesEscritura[i]);
        free(arregloPipesLectura[i]);
    }
    //Se escribe el archivo de salida
    escribirSalida(nombreSalida,estructuraFinal,N);
    free(estructuraFinal); //Se libera la memoria de el arreglo de estructura final
    return 0;
}
