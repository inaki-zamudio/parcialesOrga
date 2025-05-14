#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <argp.h>
#include <math.h>

#include "utils.h"
#include "ej2.h"


int16_t* filtro_c (const int16_t* entrada, unsigned size) {

	int16_t* salida = (int16_t*) malloc(2*size);
	
	for (unsigned i = 0; i < size/8; i++){

		for (unsigned j = 0; j < 8; j++){

			if(j < 4)
				salida[8*i+j] = (int16_t)(((int32_t)entrada[8*i+j]   + (int32_t)entrada[(i+1)*8-1-j])   * ((int32_t)entrada[8*i+j]   - (int32_t)entrada[8*i+j+4]));
			else
				salida[8*i+j] = (int16_t)(((int32_t)entrada[8*i+j-4] - (int32_t)entrada[(i+1)*8-1-j+4]) * ((int32_t)entrada[8*i+j-4] + (int32_t)entrada[8*i+j]));
		}
	}
	return salida;  
}