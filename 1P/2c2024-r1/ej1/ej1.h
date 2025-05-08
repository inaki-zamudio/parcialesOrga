#pragma once
#include <stdbool.h>
#include <stdint.h>

/**
 * Una unidad de ataque (tanques, barcos de batalla, etc.) en nuestro videojuego.
 *
 * Campos:
 *   - clase:          El nombre de la clase.
 *   - combustible:    La cantidad de combustible disponible de la unidad.
 *   - references:     La cantidad de referencias a la unidad en el mapa.
 */
typedef struct {
	char clase[11];       //asmdef_offset:ATTACKUNIT_CLASE
	uint16_t combustible; //asmdef_offset:ATTACKUNIT_COMBUSTIBLE
	uint8_t references;   //asmdef_offset:ATTACKUNIT_REFERENCES
} attackunit_t; //asmdef_size:ATTACKUNIT_SIZE

typedef attackunit_t *mapa_t[255][255];

/**
 * Marca el ejercicio 1A como hecho (`true`) o pendiente (`false`).
 *
 * Funciones a implementar:
 *   - optimizar
 */
extern bool EJERCICIO_1A_HECHO;

/**
 * Marca el ejercicio 1B como hecho (`true`) o pendiente (`false`).
 *
 * Funciones a implementar:
 *   - contarCombustibleAsignado
 */
extern bool EJERCICIO_1B_HECHO;

/**
 * Marca el ejercicio 1B como hecho (`true`) o pendiente (`false`).
 *
 * Funciones a implementar:
 *   - modificarUnidad
 */
extern bool EJERCICIO_1C_HECHO;

/**
 * OPCIONAL: implementar en C
 */
void optimizar(mapa_t mapa, attackunit_t* compartida, uint32_t (*fun_hash)(attackunit_t*));

/**
 * OPCIONAL: implementar en C
 */
uint32_t contarCombustibleAsignado(mapa_t mapa, uint16_t (*fun_combustible)(char*));

/**
 * OPCIONAL: implementar en C
 */
void modificarUnidad(mapa_t mapa, uint8_t x, uint8_t y, void (*modificar_t)(attackunit_t*));