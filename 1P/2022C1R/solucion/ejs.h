#ifndef EJS_H
#define EJS_H

#include <stdio.h> 		//encabezado de funciones de entrada y salida fopen, fclose, fgetc, printf, fprintf ...
#include <stdlib.h>		//biblioteca estándar, atoi, atof, rand, srand, abort, exit, system, NULL, malloc, calloc, realloc...
#include <stdint.h>		//contiene la definición de tipos enteros ligados a tamaños int8_t, int16_t, uint8_t,...
#include <ctype.h>		//contiene funciones relacionadas a caracteres, isdigit, islower, tolower...
#include <string.h>		//contiene las funciones relacionadas a strings, memcmp, strcat, memset, memmove, strlen,strstr...
#include <math.h>		//define funciones matemáticas como cos, sin, abs, sqrt, log...
#include <stdbool.h>	//contiene las definiciones de datos booleanos, true (1), false (0)
#include <unistd.h>		//define constantes y tipos standard, NULL, R_OK, F_OK, STDIN_FILENO, STDOUT_FILENO, STDERR_FILENO...
#include <assert.h>		//provee la macro assert que evalúa una condición, y si no se cumple provee información diagnóstica y aborta la ejecución

//*************************************
//Declaración de estructuras
//*************************************

typedef struct letters_quantity {
	uint8_t consonants_qty;
	char* word;
	uint8_t vowels_qty;
} letters_quantity_t;


//*******************************
//Declaración de funciones de ej1
//*******************************

/* Cuenta las vocales de una palabra */
uint8_t countVowels(char* word);


/* Crea un array de letters quantity vacio  */ 
letters_quantity_t* createLettersQuantityArray(uint8_t size);

/* Agrega al arreglo el vowels_quantity */
void addToArray(char* word, letters_quantity_t wq_array[], uint8_t position);

/* Libera la memoria */
void cleanWQArray(letters_quantity_t wq_array[], uint8_t size);

/* Recorre el arreglo y devuelve la palabra con más vocales */
char* getMaxVowels(letters_quantity_t* wq_array, uint8_t array_size);

/* Imprime el contenido del arreglo */
void arrayPrint(letters_quantity_t* wq_array, uint8_t size, FILE* pFile);
#endif
