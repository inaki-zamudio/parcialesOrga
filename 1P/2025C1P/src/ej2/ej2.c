#include "ej2.h"

#include <string.h>

// OPCIONAL: implementar en C
void invocar_habilidad(void* carta_generica, char* habilidad) {
	card_t* carta = carta_generica;
	while (carta != NULL) {
		for (uint16_t i = 0; i < carta->__dir_entries; i++) {
			if (strcmp(carta->__dir[i]->ability_name, habilidad) == 0) {
				((ability_function_t*)carta->__dir[i]->ability_ptr)(carta);
				return;
			}
		}
		carta = (card_t*)carta->__archetype;
	}
}
