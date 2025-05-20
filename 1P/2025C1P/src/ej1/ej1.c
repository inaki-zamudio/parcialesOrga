#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ej1.h"

/**
 * Marca el ejercicio 1A como hecho (`true`) o pendiente (`false`).
 *
 * Funciones a implementar:
 *   - init_fantastruco_dir
 */
bool EJERCICIO_1A_HECHO = true;

// OPCIONAL: implementar en C
void init_fantastruco_dir(fantastruco_t* card) {
    card->__dir = malloc(2*sizeof(directory_entry_t*));
    card->__dir[0] = create_dir_entry("sleep", sleep);
    card->__dir[1] = create_dir_entry("wakeup", wakeup);
    card->__dir_entries = 2;
}

/**
 * Marca el ejercicio 1A como hecho (`true`) o pendiente (`false`).
 *
 * Funciones a implementar:
 *   - summon_fantastruco
 */
bool EJERCICIO_1B_HECHO = true;

// OPCIONAL: implementar en C
fantastruco_t* summon_fantastruco() {
    fantastruco_t* card = malloc(sizeof(fantastruco_t));
    init_fantastruco_dir(card);
    card->__archetype = NULL;
    card->face_up = 1;
    return card;
}
