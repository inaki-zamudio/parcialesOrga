#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <ctype.h>
#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <float.h>
#include "ejs.h"
#include "str.h"

#define ROLL_LENGTH 100

#define RUN(filename, action) pfile=fopen(filename,"a"); action; fclose(pfile);
#define NL(filename) pfile=fopen(filename,"a"); fprintf(pfile,"\n"); fclose(pfile);

char *filename_ej1 =  "salida.propios.ej1.txt";
void test_ej1(char* filename);


int main() {
	srand(0);
    remove(filename_ej1);
	test_ej1(filename_ej1);
	return 0;
}


static uint32_t x[ROLL_LENGTH];
static float f[ROLL_LENGTH];

void shuffle(uint32_t max){
	for(int i = 0; i < ROLL_LENGTH; i++){
		x[i] = (uint32_t) rand() % max;
        f[i] = ((float)rand()/(float)(RAND_MAX)) * max;
	}
}

#define LENGTH 11

char* strings[LENGTH] = {"mikasa","eren","murcielago","erwin","shopping","reiner","aventura","jean","sasha","hanji",""};

void test_ej1(char* filename) {
    FILE* pfile;
    char* a;

    RUN(filename, fprintf(pfile, "== Ejercicio 1 ==\n");) NL(filename)

    // Agregar
    RUN(filename, fprintf(pfile, "Test countVowels - Cuenta Vocales\n");) NL(filename)
    a = strings[0];
    RUN(filename, fprintf(pfile, "%i - ", countVowels(a));) RUN(filename, strPrint(a,pfile);) NL(filename)
    a = strings[1];
    RUN(filename, fprintf(pfile, "%i - ", countVowels(a));) RUN(filename, strPrint(a,pfile);) NL(filename)
    a = strings[2];
    RUN(filename, fprintf(pfile, "%i - ", countVowels(a));) RUN(filename, strPrint(a,pfile);) NL(filename)
    a = strings[10];
    RUN(filename, fprintf(pfile, "%i - ", countVowels(a));) RUN(filename, strPrint(a,pfile);) NL(filename)
    NL(filename)
    // Get
    RUN(filename, fprintf(pfile, "Test createLettersQuantityArray - Crea un arreglo de Letras Cantidad\n");) NL(filename)
    
    // Test 1 - Setup
    int size = 1;
    letters_quantity_t* arr = createLettersQuantityArray(size);
    addToArray(strings[0], arr, 0);
    RUN(filename, arrayPrint(arr,size,pfile);) NL(filename)
    cleanWQArray(arr, size);

    // Test 2 - Setup
    size = 5;
    arr = createLettersQuantityArray(size);
    addToArray(strings[0], arr, 0);
    addToArray(strings[2], arr, 2);
    addToArray(strings[4], arr, 4);
    RUN(filename, arrayPrint(arr,size,pfile);) NL(filename)
    cleanWQArray(arr, size);

    RUN(filename, fprintf(pfile, "Test getMaxVowels - Obtiene la palabra con más consonantes \n");) NL(filename)
    
    // Test 3 - Setup
    size = 3;
    arr = createLettersQuantityArray(size);
    addToArray(strings[0], arr, 0);
    addToArray(strings[1], arr, 1);
    addToArray(strings[2], arr, 2);
    char* masVocales = getMaxVowels(arr, size);
    RUN(filename, arrayPrint(arr,size,pfile);) RUN(filename, strPrint(masVocales,pfile);) NL(filename)
    free(masVocales);
    cleanWQArray(arr, size);


    size = 10;
    arr = createLettersQuantityArray(size);
    addToArray(strings[5], arr, 5);
    masVocales = getMaxVowels(arr, size);
    RUN(filename, arrayPrint(arr,size,pfile);) RUN(filename, strPrint(masVocales,pfile);) NL(filename)
    free(masVocales);
    cleanWQArray(arr, size);
    /*

    letters_quantity_t* arr = createLettersQuantityArray(30);
    
    letters_quantity_t t = createLettersQuantity(strings[2]);
    
    printf("%i\n", t.vowels_qty);
    printf("%s\n", t.word);
    printf("%i\n", t.consonants_qty);

    addToArray(t, arr, 1);
    addToArray(t2, arr, 3);
    addToArray(t3, arr, 9);
    RUN(filename, fprintf(pfile, "%i - ", strArrayGetSize(a));) RUN(filename, strArrayPrint(a,pfile);) NL(filename)
    data = strArrayGet(a,0);
    RUN(filename, fprintf(pfile, "%s\n", (char*)data);)
    data = strArrayGet(a,1);
    RUN(filename, fprintf(pfile, "%s\n", (char*)data);)
    data = strArrayGet(a,2);
    RUN(filename, fprintf(pfile, "%s\n", (char*)data);)
    data = strArrayGet(a,3);
    RUN(filename, fprintf(pfile, "%s\n", (char*)data);)
    */
    //NL(filename)

    // Quitar
    /*RUN(filename, fprintf(pfile, "Obtiene la palabra con más vocales\n");) NL(filename)
    data = strArrayRemove(a,0);
    RUN(filename, fprintf(pfile, "%i - ", getMaxVowels(a,10));) RUN(filename, strArrayPrint(a,pfile);) RUN(filename, fprintf(pfile, " - %s\n", (char*)data);)
    strDelete((char*)data);
    data = strArrayRemove(a,1);
    RUN(filename, fprintf(pfile, "%i - ", getMaxVowels(a,10));) RUN(filename, strArrayPrint(a,pfile);) RUN(filename, fprintf(pfile, " - %s\n", (char*)data);)
    strDelete((char*)data);
    data = strArrayRemove(a,0);
    RUN(filename, fprintf(pfile, "%i - ", getMaxVowels(a,10));) RUN(filename, strArrayPrint(a,pfile);) RUN(filename, fprintf(pfile, " - %s\n", (char*)data);)
    strDelete((char*)data);
    NL(filename)*/
}


