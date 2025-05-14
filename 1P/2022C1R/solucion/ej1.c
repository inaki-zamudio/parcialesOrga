#include "ejs.h"
#include "str.h"

#ifdef ENABLE_C
/* Cuenta las vocales de una palabra */
uint8_t countVowels(char* word) {
    char* s = word;
    char c = *s;
    uint8_t qty = 0;
    while (c != 0) {
        if (c == 0x61 || c == 0x65 || c ==  0x69 || c == 0x6F || c == 0x75) {
            qty += 1;
        }
        s += 1;
        c = *s;
    }
    return qty;
}


/* Crea un array de letters quantity vacio  */ 
letters_quantity_t* createLettersQuantityArray(uint8_t size) {
    letters_quantity_t* arr = malloc(sizeof(letters_quantity_t)*size);
    for (int i=0; i < size; i++) {
        arr[i] = (letters_quantity_t) { 0 } ;
    }
    return arr;    
}

/* Recorre el arreglo y devuelve una copia de palabra con más vocales */
char* getMaxVowels(letters_quantity_t* wq_array, uint8_t array_size) {
    if (array_size == 0)
        return NULL;

    uint8_t max_vowels = wq_array[0].vowels_qty;
    char* max_word = wq_array[0].word;
    for (int i=1; i < array_size; i++) {
        if (wq_array[i].vowels_qty > max_vowels) {
            max_vowels = wq_array[i].vowels_qty;
            max_word = wq_array[i].word;
        }
    }
    return strClone(max_word);
}
#endif
/************ Funciones Dadas ***********************/

/* Agrega al arreglo el vowels_quantity */
void addToArray(char* word, letters_quantity_t wq_array[], uint8_t position) {
    uint8_t vowels = countVowels(word);
    wq_array[position].vowels_qty = vowels;
    wq_array[position].word = strClone(word);
    wq_array[position].consonants_qty = strLen(word)-vowels;
}

/* Libera la memoria */
void cleanWQArray(letters_quantity_t wq_array[], uint8_t size) {
    for (int i = 0 ; i < size ; i++) {
        free(wq_array[i].word);
    }
    free(wq_array);
}


void arrayPrint(letters_quantity_t* wq_array, uint8_t size, FILE* pFile) {
    fprintf(pFile, "[");
    for(int i=0; i< size - 1; i++) {
        if (wq_array[i].word != NULL) {
            fprintf(pFile, "consonats: %i - word: ", wq_array[i].consonants_qty);
            strPrint(wq_array[i].word, pFile);
            fprintf(pFile, "- vowels: %i,", wq_array[i].vowels_qty);
        } else {
            fprintf(pFile, "NULL,");
        }

    }
    if(size >= 1) {
        int i = size - 1 ;
        if (wq_array[i].word != NULL) {
            
            fprintf(pFile, "consonats: %i - word: ", wq_array[i].consonants_qty);
            strPrint(wq_array[i].word, pFile);
            fprintf(pFile, " - vowels: %i", wq_array[i].vowels_qty);
        } else {
            fprintf(pFile, "NULL");
        }
    }
    fprintf(pFile, "]");
}


