#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <ctype.h>
#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <float.h>
#include "ej2.h"
#include "utils.h"

#define ROLL_LENGTH 100

#define RUN(filename, action) pfile=fopen(filename,"a"); action; fclose(pfile);
#define NL(filename) pfile=fopen(filename,"a"); fprintf(pfile,"\n"); fclose(pfile);

char *filename_ej2 =  "salida.propios.ej2.txt";
void test_ej2(char* filename);
void generarEntrada(int16_t* entrada, unsigned size);

int main() {
	srand(0);
    remove(filename_ej2);
	test_ej2(filename_ej2);
	return 0;
}

void generarEntrada(int16_t* entrada, unsigned size){
	for (unsigned i = 0; i < size; i++){
		int a=rand();
		entrada[i]=a;
	}
}
	
void test_ej2(char* filename) {
    FILE* pfile;
    int16_t* entrada;
    int16_t* salida;
    int16_t* salida2;
    unsigned size;

    RUN(filename, fprintf(pfile, "== Ejercicio 2 ==\n");) NL(filename)
    
    // Entrada a mano
    RUN(filename, fprintf(pfile, "Entrada size 8\n");) NL(filename)

	int16_t arreglo[8]={1,2,3,4,5,6,7,8};
	salida= (int16_t*) filtro_c(arreglo, 8);
	salida2= (int16_t*) filtro(arreglo, 8);
	RUN(filename, printSalida(pfile, salida, salida2,4);)
	free(salida);
	free(salida2);
	NL(filename)

    // Entrada chica
    RUN(filename, fprintf(pfile, "Entrada size 16\n");) NL(filename)
    size=16;
    entrada=(int16_t*)malloc(size*2);
    generarEntrada(entrada, size);
    salida = (int16_t*) filtro_c(entrada,size);
    salida2 = (int16_t*) filtro(entrada,size);
    RUN(filename, printSalida(pfile, salida, salida2,size);)
    free(salida);
    free(entrada);
    free(salida2);
    NL(filename)

    // Entrada mediana
    RUN(filename, fprintf(pfile, "Entrada Size 32\n");) NL(filename)
    size=32;
    entrada=(int16_t*)malloc(size*2);
    generarEntrada(entrada, size);
    salida = (int16_t*) filtro_c(entrada,size);
    salida2 = (int16_t*) filtro(entrada,size);
    RUN(filename, printSalida(pfile, salida,salida2,size);)
    free(salida);
    free(entrada);
    free(salida2);
    NL(filename)
    
    // Entrada grande
    RUN(filename, fprintf(pfile, "Entrada Size 136\n");) NL(filename)
    size=136;
    entrada=(int16_t*)malloc(size*2);
    generarEntrada(entrada, size);
    salida = (int16_t*) filtro_c(entrada,size);
    salida2 = (int16_t*) filtro(entrada,size);
    RUN(filename, printSalida(pfile, salida,salida2,size-3);)
    free(salida);
    free(entrada);
    free(salida2);
    NL(filename) 
}
