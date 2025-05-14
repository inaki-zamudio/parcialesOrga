#include "ej1.h"

nodo_display_list_t* inicializar_nodo(
  uint8_t (*primitiva)(uint8_t x, uint8_t y, uint8_t z_size),
  uint8_t x, uint8_t y, nodo_display_list_t* siguiente) {
    nodo_display_list_t* nodo = malloc(sizeof(nodo_display_list_t));
    nodo->primitiva = primitiva;
    nodo->x = x;
    nodo->y = y;
    nodo->z = 255;
    nodo->siguiente = siguiente;
    return nodo;
}

ordering_table_t* inicializar_OT(uint8_t table_size) {
  ordering_table_t* ot = malloc(sizeof(ordering_table_t));
  ot->table = malloc(table_size * sizeof(nodo_display_list_t*));
  ot->table_size = table_size;
  for (uint8_t i = 0; i < table_size; i++) {
    ot->table[i] = NULL;
  }
  return ot;
}

void calcular_z(nodo_display_list_t* nodo, uint8_t z_size) {
  if (nodo != NULL && nodo->primitiva != NULL) {
    nodo->z = nodo->primitiva(nodo->x, nodo->y, z_size);
  }
}

void ordenar_display_list(ordering_table_t* ot, nodo_display_list_t* display_list) {
  while (display_list != NULL) {
    calcular_z(display_list, ot->table_size);
    uint8_t z_index = display_list->z % ot->table_size;

    nodo_display_list_t* siguiente = display_list->siguiente; // Guardar el siguiente nodo
    display_list->siguiente = ot->table[z_index];
    ot->table[z_index] = display_list;
    display_list = siguiente; // Avanzar al siguiente nodo original
  }
}
