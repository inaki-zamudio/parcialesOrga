#include <stdbool.h>
#include <stdint.h>

/**
 * El ancho del campo de juego
 */
#define ANCHO_CAMPO 10
/**
 * El alto del campo de juego
 */
#define ALTO_CAMPO 5

/**
 * El identificador de las cartas cuyo dueño es el jugador rojo
 */
#define JUGADOR_ROJO 1
/**
 * El identificador de las cartas cuyo dueño es el jugador azul
 */
#define JUGADOR_AZUL 2

/**
 * Una carta (posiblemente) en juego.
 *
 * Posee un nombre, una cantidad de puntos de vida y un dueño.
 */
typedef struct carta {
	bool en_juego;    // asmdef_offset:carta.en_juego
	char nombre[12];  // asmdef_offset:carta.nombre
	uint16_t vida;    // asmdef_offset:carta.vida
	uint8_t jugador;  // asmdef_offset:carta.jugador
} carta_t;  // asmdef_size:carta.SIZE

/**
 * Una instancia del juego en proceso.
 *
 * Posee las manos de ambos jugadores humanos y el campo de juego
 */
typedef struct tablero {
	carta_t* mano_jugador_rojo;               // asmdef_offset:tablero.mano_jugador_rojo
	carta_t* mano_jugador_azul;               // asmdef_offset:tablero.mano_jugador_azul
	carta_t* campo[ALTO_CAMPO][ANCHO_CAMPO];  // asmdef_offset:tablero.campo
} tablero_t;  // asmdef_size:tablero.SIZE

/**
 * La implementación de una acción del juego.
 *
 * Toma la carta destino y una instancia del juego, realiza modificaciones
 * arbitrarias a ambos.
 */
typedef void accion_fn_t(tablero_t* tablero, carta_t* carta);

/**
 * Una acción del juego.
 *
 * Posee una pieza de código a invocar, una carta destino y (opcionalmente) la
 * acción que sigue luego de ésta
 */
typedef struct accion {
	accion_fn_t* invocar;      // asmdef_offset:accion.invocar
	carta_t* destino;          // asmdef_offset:accion.destino
	struct accion* siguiente;  // asmdef_offset:accion.siguiente
} accion_t;  // asmdef_size:accion.SIZE

/**
 * Dada una secuencia de acciones determinar si hay alguna cuya carta tenga un
 * nombre idéntico (mismos contenidos, no mismo puntero) al pasado por
 * parámetro.
 *
 * El resultado es un valor booleano, la representación de los booleanos de C es
 * la siguiente:
 *   - El valor `0` es `false`
 *   - Cualquier otro valor es `true`
 */
bool hay_accion_que_toque(accion_t* accion, char* nombre);

/**
 * Invoca las acciones que fueron encoladas en la secuencia proporcionada en el
 * primer parámetro.
 *
 * A la hora de procesar una acción esta sólo se invoca si la carta destino
 * sigue en juego.
 *
 * Luego de invocar una acción, si la carta destino tiene cero puntos de vida,
 * se debe marcar ésta como fuera de juego.
 *
 * Las funciones que implementan acciones de juego tienen la siguiente firma:
 * ```c
 * void mi_accion(tablero_t* tablero, carta_t* carta);
 * ```
 * - El tablero a utilizar es el pasado como parámetro
 * - La carta a utilizar es la carta destino de la acción (`accion->destino`)
 *
 * Las acciones se deben invocar en el orden natural de la secuencia (primero
 * la primera acción, segundo la segunda acción, etc). Las acciones asumen este
 * orden de ejecución.
 */
void invocar_acciones(accion_t* accion, tablero_t* tablero);

/**
 * Cuenta la cantidad de cartas rojas y azules en el tablero.
 *
 * Dado un tablero revisa el campo de juego y cuenta la cantidad de cartas
 * correspondientes al jugador rojo y al jugador azul. Este conteo incluye
 * tanto a las cartas en juego cómo a las fuera de juego (siempre que estén
 * visibles en el campo).
 *
 * Se debe considerar el caso de que el campo contenga cartas que no pertenecen
 * a ninguno de los dos jugadores.
 *
 * Las posiciones libres del campo tienen punteros nulos en lugar de apuntar a
 * una carta.
 *
 * El resultado debe ser escrito en las posiciones de memoria proporcionadas
 * como parámetro.
 */
void contar_cartas(tablero_t* tablero, uint32_t* cant_rojas, uint32_t* cant_azules);

/**
 * Marca el ejercicio 1 como hecho (`true`) o pendiente (`false`).
 *
 * Funciones a implementar:
 *   - hay_accion_que_toque
 */
extern bool EJERCICIO_1_HECHO;

/**
 * Marca el ejercicio 2 como hecho (`true`) o pendiente (`false`).
 *
 * Funciones a implementar:
 *   - invocar_acciones
 */
extern bool EJERCICIO_2_HECHO;

/**
 * Marca el ejercicio 3 como hecho (`true`) o pendiente (`false`).
 *
 * Funciones a implementar:
 *   - contar_cartas
 */
extern bool EJERCICIO_3_HECHO;
