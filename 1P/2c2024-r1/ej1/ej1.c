#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ej1.h"

/**
 * Marca el ejercicio 1A como hecho (`true`) o pendiente (`false`).
 *
 * Funciones a implementar:
 *   - es_indice_ordenado
 */
bool EJERCICIO_1A_HECHO = true;

/**
 * Marca el ejercicio 1B como hecho (`true`) o pendiente (`false`).
 *
 * Funciones a implementar:
 *   - contarCombustibleAsignado
 */
bool EJERCICIO_1B_HECHO = false;

/**
 * Marca el ejercicio 1B como hecho (`true`) o pendiente (`false`).
 *
 * Funciones a implementar:
 *   - modificarUnidad
 */
bool EJERCICIO_1C_HECHO = false;

/**
 * OPCIONAL: implementar en C
 */
void optimizar(mapa_t mapa, attackunit_t* compartida, uint32_t (*fun_hash)(attackunit_t*)) {
    uint32_t hash_compartida = fun_hash(compartida);
    for (int i = 0; i < 255; i++) {
        for (int j = 0; j < 255; j++) {
            
            if (mapa[i][j] == compartida) {
                continue;
            }

            if (mapa[i][j] == NULL) {
                continue;
            } 
            
            uint32_t hash_actual = fun_hash(mapa[i][j]);
            if (hash_compartida != hash_actual) {
                continue;
            }
            // si no, hago la opti
            mapa[i][j]->references--;
            if (mapa[i][j]->references == 0) {
                free(mapa[i][j]);
            }

            mapa[i][j] = compartida;
            compartida->references++;
        }
    }
}

/**
 * OPCIONAL: implementar en C
 */
uint32_t contarCombustibleAsignado(mapa_t mapa, uint16_t (*fun_combustible)(char*)) {
    
}

/**
 * OPCIONAL: implementar en C
 */
void modificarUnidad(mapa_t mapa, uint8_t x, uint8_t y, void (*fun_modificar)(attackunit_t*)) {
   
}
