#include "ej1.h"
// inicializa un nodo_display_list_t
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
  if (table_size == 0) { 
    ot->table = NULL;
  }
  else {
    ot->table = calloc(table_size, sizeof(nodo_ot_t*));
    ot->table_size = table_size;
  }
  return ot;
}

void calcular_z(nodo_display_list_t* nodo, uint8_t z_size) {
  nodo_display_list_t* act = nodo;
  while (act != NULL) {
    act->z = act->primitiva(act->x, act->y, z_size);
    act = act->siguiente;
  }
}

void ordenar_display_list(ordering_table_t* ot, nodo_display_list_t* display_list) {
  
}
