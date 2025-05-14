#include "ej2.h"

/**
 * Marca el ejercicio 2A como hecho (`true`) o pendiente (`false`).
 *
 * Funciones a implementar:
 *   - ej2a
 */
bool EJERCICIO_2A_HECHO = false;

/**
 * Dada una imagen origen escribe en el destino `scale * px + offset` por cada
 * píxel en la imagen.
 *
 * Parámetros:
 *   - dst_depth: La imagen destino (mapa de profundidad). Está en escala de
 *                grises a 32 bits con signo por canal.
 *   - src_depth: La imagen origen (mapa de profundidad). Está en escala de
 *                grises a 8 bits sin signo por canal.
 *   - scale:     El factor de escala. Es un entero con signo de 32 bits.
 *                Multiplica a cada pixel de la entrada.
 *   - offset:    El factor de corrimiento. Es un entero con signo de 32 bits.
 *                Se suma a todos los píxeles luego de escalarlos.
 *   - width:     El ancho en píxeles de `src_depth` y `dst_depth`.
 *   - height:    El alto en píxeles de `src_depth` y `dst_depth`.
 */
void ej2a(
	int32_t* dst_depth,
	uint8_t* src_depth,
	int32_t scale, int32_t offset,
	uint32_t width, uint32_t height
) {
}

/**
 * Marca el ejercicio 2B como hecho (`true`) o pendiente (`false`).
 *
 * Funciones a implementar:
 *   - ej2b
 */
bool EJERCICIO_2B_HECHO = false;

/**
 * Dadas dos imágenes de origen (`a` y `b`) en conjunto con sus mapas de
 * profundidad escribe en el destino el pixel de menor profundidad por cada
 * píxel de la imagen. En caso de empate se escribe el píxel de `b`.
 *
 * Parámetros:
 *   - dst:     La imagen destino. Está a color (RGBA) en 8 bits sin signo por
 *              canal.
 *   - a:       La imagen origen A. Está a color (RGBA) en 8 bits sin signo por
 *              canal.
 *   - depth_a: El mapa de profundidad de A. Está en escala de grises a 32 bits
 *              con signo por canal.
 *   - b:       La imagen origen B. Está a color (RGBA) en 8 bits sin signo por
 *              canal.
 *   - depth_b: El mapa de profundidad de B. Está en escala de grises a 32 bits
 *              con signo por canal.
 *   - width:  El ancho en píxeles de todas las imágenes parámetro.
 *   - height: El alto en píxeles de todas las imágenes parámetro.
 */
void ej2b(
	rgba_t* dst,
	rgba_t* a, int32_t* depth_a,
	rgba_t* b, int32_t* depth_b,
	uint32_t width, uint32_t height
) {
}
