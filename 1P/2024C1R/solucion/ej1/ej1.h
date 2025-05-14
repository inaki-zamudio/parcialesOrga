#pragma once
#include <stdbool.h>
#include <stdint.h>

/**
 * El tipo de los `texto_cualquiera_t` que son cadenas de caracteres clásicas.
 */
#define TEXTO_LITERAL       0

/**
 * El tipo de los `texto_cualquiera_t` que son concatenaciones de textos.
 */
#define TEXTO_CONCATENACION 1

/**
 * Un texto que puede estar compuesto de múltiples partes. Dependiendo del
 * campo `tipo` debe ser interpretado como un `texto_literal_t` o un
 * `texto_concatenacion_t`.
 *
 * Campos:
 *   - tipo: El tipo de `texto_cualquiera_t` en cuestión (literal o
 *           concatenación).
 *   - usos: Cantidad de instancias de `texto_cualquiera_t` que están usando a
 *           este texto.
 */
typedef struct {
	uint32_t tipo;
	uint32_t usos;
	uint64_t unused0; // Reservamos espacio
	uint64_t unused1; // Reservamos espacio
} texto_cualquiera_t;

/**
 * Un texto que tiene una única parte la cual es una cadena de caracteres
 * clásica.
 *
 * Campos:
 *   - tipo:      El tipo del texto. Siempre `TEXTO_LITERAL`.
 *   - usos:      Cantidad de instancias de `texto_cualquiera_t` que están
 *                usando a este texto.
 *   - tamanio:   El tamaño del texto.
 *   - contenido: El texto en cuestión como un array de caracteres.
 */
typedef struct {
	uint32_t tipo;
	uint32_t usos;
	uint64_t tamanio;
	const char* contenido;
} texto_literal_t;

/**
 * Un texto que es el resultado de concatenar otros dos `texto_cualquiera_t`.
 *
 * Campos:
 *   - tipo:      El tipo del texto. Siempre `TEXTO_CONCATENACION`.
 *   - usos:      Cantidad de instancias de `texto_cualquiera_t` que están
 *                usando a este texto.
 *   - izquierda: El tamaño del texto.
 *   - derecha:   El texto en cuestión como un array de caracteres.
 */
typedef struct {
	uint32_t tipo;
	uint32_t usos;
	texto_cualquiera_t* izquierda;
	texto_cualquiera_t* derecha;
} texto_concatenacion_t;

/**
 * Muestra un `texto_cualquiera_t` en la pantalla.
 *
 * Parámetros:
 *   - texto: El texto a imprimir.
 */
void texto_imprimir(texto_cualquiera_t* texto);

/**
 * Libera un `texto_cualquiera_t` pasado por parámetro. Esto hace que toda la
 * memoria usada por ese texto (y las partes que lo componen) sean devueltas al
 * sistema operativo.
 *
 * Si una cadena está siendo usada por otra entonces ésta no se puede liberar.
 * `texto_liberar` notifica al usuario de esto devolviendo `false`. Es decir:
 * `texto_liberar` devuelve un booleando que representa si la acción pudo
 * llevarse a cabo o no.
 *
 * Parámetros:
 *   - texto: El texto a liberar.
 */
bool texto_liberar(texto_cualquiera_t* texto);

/**
 * Marca el ejercicio 1A como hecho (`true`) o pendiente (`false`).
 *
 * Funciones a implementar:
 *   - texto_literal
 *   - texto_concatenar
 */
extern bool EJERCICIO_1A_HECHO;

/**
 * Crea un `texto_literal_t` que representa la cadena pasada por parámetro.
 *
 * Debe calcular la longitud de esa cadena.
 *
 * El texto resultado no tendrá ningún uso (dado que es un texto nuevo).
 *
 * Parámetros:
 *   - texto: El texto que debería ser representado por el literal a crear.
 */
texto_literal_t* texto_literal(const char* texto);

/**
 * Crea un `texto_concatenacion_t` que representa la concatenación de ambos
 * parámetros.
 *
 * Los textos `izquierda` y `derecha` serán usadas por el resultado, por lo
 * que sus contadores de usos incrementarán.
 *
 * Parámetros:
 *   - izquierda: El texto que debería ir a la izquierda.
 *   - derecha:   El texto que debería ir a la derecha.
 */
texto_concatenacion_t* texto_concatenar(texto_cualquiera_t* izquierda, texto_cualquiera_t* derecha);

/**
 * Marca el ejercicio 1B como hecho (`true`) o pendiente (`false`).
 *
 * Funciones a implementar:
 *   - texto_tamanio_total
 */
extern bool EJERCICIO_1B_HECHO;

/**
 * Calcula el tamaño total de un `texto_cualquiera_t`. Es decir, suma todos los
 * campos `tamanio` involucrados en el mismo.
 *
 * Parámetros:
 *   - texto: El texto en cuestión.
 */
uint64_t texto_tamanio_total(texto_cualquiera_t* texto);

/**
 * Marca el ejercicio 1C como hecho (`true`) o pendiente (`false`).
 *
 * Funciones a implementar:
 *   - texto_chequear_tamanio
 */
extern bool EJERCICIO_1C_HECHO;

/**
 * Chequea si los tamaños de todos los nodos literales internos al parámetro
 * corresponden al tamaño de la cadenas que apuntadan.
 *
 * Es decir: si los campos `tamanio` están bien calculados.
 *
 * Parámetros:
 *   - texto: El texto verificar.
 */
bool texto_chequear_tamanio(texto_cualquiera_t* texto);
