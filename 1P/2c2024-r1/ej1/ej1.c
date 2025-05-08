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
bool EJERCICIO_1A_HECHO = false;

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
bool EJERCICIO_1C_HECHO = true;

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
    uint32_t res = 0;
    for (int i = 0; i < 255; i++) {
        for (int j = 0; j < 255; j++) {
            if (mapa[i][j] == NULL) {
                continue;
            }
            uint16_t comb_base = fun_combustible(mapa[i][j]->clase);
            res += mapa[i][j]->combustible - comb_base;
        }
    }
    return res;
}

/**
 * OPCIONAL: implementar en C
 */
void modificarUnidad(mapa_t mapa, uint8_t x, uint8_t y, void (*fun_modificar)(attackunit_t*)) {
    if (mapa[x][y] == NULL){
        return;
    }
    if (mapa[x][y]->references == 1){
    fun_modificar(mapa[x][y]);
   } else {
    attackunit_t* nueva_unidad = malloc(sizeof(attackunit_t));
    *nueva_unidad = *mapa[x][y];
    mapa[x][y]->references--;
    nueva_unidad->references = 1;
    fun_modificar(nueva_unidad);
    mapa[x][y] = nueva_unidad;
   }
}
