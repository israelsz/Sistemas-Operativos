#ifndef Funciones_h_
#define Funciones_h_
#include<stdio.h>

//Estructura de una particula

typedef struct Particula Particula;
struct Particula{
    int j; //Posicion de la particula
    int Ep; //Energia potencial
};

//Cabeceras de funciones
Particula crearParticula(int j, int Ep);
void mostrarParticula(Particula p);
Particula * archivoParticulas(FILE *f, int *largoBombardeos);
void mostrarArregloFloat(float *A,int largo);
float energiaAplicada(float Ei,int i,Particula p, int N);
FILE * leerArchivo(char* nombreArchivo);
void escribirSalida(char *nombreSalida, float *Energy, int N);
extern void niceprint(int N, float *Energy, float Maximum); //Nice Print
float *aplicarEnergia(int Linicio, int Lfinal, int N, char *nombreArchivo, int cantidadParticulasTotales);
int encontrarIndiceMayor(float* energy, int N);
#endif

