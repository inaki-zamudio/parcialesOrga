#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>

#include "ejs.h"

int main (void){
	/* Acá pueden realizar sus propias pruebas  - Ejemplo: */
	
	
	printf("%i\n", countVowels("casa"));

	letters_quantity_t* arr = createLettersQuantityArray(10);
	addToArray("musicuentos", arr, 1);
	addToArray("casaiiiiiiiii", arr, 3);
	addToArray("iinteresante", arr, 9);

	char* maxWord = getMaxVowels(arr, 10);
	printf("%s\n", maxWord);
	
	free(maxWord);
	cleanWQArray(arr,10);
	
	return 0;    
}


