#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ej1.h"

/**
 * Muestra un `texto_cualquiera_t` en la pantalla.
 *
 * Parámetros:
 *   - texto: El texto a imprimir.
 */
void texto_imprimir(texto_cualquiera_t* texto) {
	if (texto->tipo == TEXTO_LITERAL) {
		texto_literal_t* literal = (texto_literal_t*) texto;
		printf("%s", literal->contenido);
	} else {
		texto_concatenacion_t* concatenacion = (texto_concatenacion_t*) texto;
		texto_imprimir(concatenacion->izquierda);
		texto_imprimir(concatenacion->derecha);
	}
}

/**
 * Libera un `texto_cualquiera_t` pasado por parámetro. Esto hace que toda la
 * memoria usada por ese texto (y las partes que lo componen) sean devueltas al
 * sistema operativo.
 *
 * Si una cadena está siendo usada por otra entonces ésta no se puede liberar.
 * `texto_liberar` notifica al usuario de esto devolviendo `false`. Es decir:
 * `texto_liberar` devuelve un booleano que representa si la acción pudo
 * llevarse a cabo o no.
 *
 * Parámetros:
 *   - texto: El texto a liberar.
 */
bool texto_liberar(texto_cualquiera_t* texto) {
	if (texto->usos != 0) {
		// Alguien está usando a este texto, aún no lo podemos liberar
		return false;
	}

	if (texto->tipo == TEXTO_CONCATENACION) {
		texto_concatenacion_t* concatenacion = (texto_concatenacion_t*) texto;
		// Vamos a dejar de usar la cadena de la izquierda
		concatenacion->izquierda->usos--;
		texto_liberar(concatenacion->izquierda);

		// Y vamos a dejar de usar la cadena de la derecha
		concatenacion->derecha->usos--;
		texto_liberar(concatenacion->derecha);
	}
	free(texto);
	return true;
}

/**
 * Marca el ejercicio 1A como hecho (`true`) o pendiente (`false`).
 *
 * Funciones a implementar:
 *   - texto_literal
 *   - texto_concatenar
 */
bool EJERCICIO_1A_HECHO = true;

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
texto_literal_t* texto_literal(const char* texto) {
	texto_literal_t* res = malloc(sizeof(texto_literal_t));
	res->tipo = 0;
	res->usos = 0;
	res->tamanio = strlen(texto);
	res->contenido = texto;
	return res;
}

/**
 * Crea un `texto_concatenacion_t` que representa la concatenación de ambos
 * parámetros.
 *
 * Los textos `izquierda` y `derecha` serán usadas por el resultado, por lo
 * que sus contadores de usos incrementarán.
 *
 * El texto resultado no tendrá ningún uso (dado que es un texto nuevo).
 *
 * Parámetros:
 *   - izquierda: El texto que debería ir a la izquierda.
 *   - derecha:   El texto que debería ir a la derecha.
 */
texto_concatenacion_t* texto_concatenar(texto_cualquiera_t* izquierda, texto_cualquiera_t* derecha) {
	texto_concatenacion_t* res = malloc(sizeof(texto_concatenacion_t));
	res->tipo = 1;
	res->usos = 0;
	res->izquierda = izquierda;
	izquierda->usos++;
	derecha->usos++;
	res->derecha = derecha;
	return res;
}

/**
 * Marca el ejercicio 1B como hecho (`true`) o pendiente (`false`).
 *
 * Funciones a implementar:
 *   - texto_tamanio_total
 */
bool EJERCICIO_1B_HECHO = true;

/**
 * Calcula el tamaño total de un `texto_cualquiera_t`. Es decir, suma todos los
 * campos `tamanio` involucrados en el mismo.
 *
 * Parámetros:
 *   - texto: El texto en cuestión.
 */
uint64_t texto_tamanio_total(texto_cualquiera_t* texto) {
	if (texto->tipo == TEXTO_LITERAL) {
		texto_literal_t* literal = (texto_literal_t*) texto;
		return literal->tamanio;
	} else {
		texto_concatenacion_t* concatenacion = (texto_concatenacion_t*) texto;
		return texto_tamanio_total(concatenacion->derecha) + texto_tamanio_total(concatenacion->izquierda);
	}
}

/**
 * Marca el ejercicio 1C como hecho (`true`) o pendiente (`false`).
 *
 * Funciones a implementar:
 *   - texto_chequear_tamanio
 */
bool EJERCICIO_1C_HECHO = true;

/**
 * Chequea si los tamaños de todos los nodos literales internos al parámetro
 * corresponden al tamaño de la cadenas que apuntadan.
 *
 * Es decir: si los campos `tamanio` están bien calculados.
 *
 * Parámetros:
 *   - texto: El texto verificar.
 */
bool texto_chequear_tamanio(texto_cualquiera_t* texto) {
	if (texto->tipo == TEXTO_LITERAL) {
		texto_literal_t* literal = (texto_literal_t*) texto;
		// ¿Cómo chequeo si un literal tiene el tamaño bien calculado?
		return strlen(literal->contenido) == literal->tamanio;
	} else {
		texto_concatenacion_t* concatenacion = (texto_concatenacion_t*) texto;
		// ¿Cómo chequeo si una concatenación tiene el tamaño de sus literales
		//  bien calculado?
		if (texto_chequear_tamanio(concatenacion->izquierda) && texto_chequear_tamanio(concatenacion->derecha)) {
			return texto_tamanio_total(concatenacion) == texto_tamanio_total(concatenacion->izquierda) + texto_tamanio_total(concatenacion->derecha);
		}
		return false;
	}
}

