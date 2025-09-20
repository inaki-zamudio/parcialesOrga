#ifndef TEST_H
#define TEST_H

#include "../test_utils/test-utils.h"
#include "./ejs.h"


//*************************************
// Declaraciones de funciones auxiliares para Tuiter
//*************************************
tuit_t *crear_tuit(char *mensaje, uint16_t favoritos, uint16_t retuits,
                   uint32_t id_autor);
usuario_t crear_usuario(uint32_t id);
feed_t *crear_feed();
publicacion_t *crear_publicacion(tuit_t *tuit_value);
void agregar_publicacion(usuario_t *user, publicacion_t *publicacion);
void agregar_publicacion_al_final(feed_t *feed, publicacion_t *publicacion);
void liberar_tuit(tuit_t *t);
void liberar_publicacion(publicacion_t *publicacion);
void liberar_feed(feed_t *lista);
void liberar_usuario(usuario_t *user);
bool tuits_iguales(tuit_t *a, tuit_t *b);
bool usuarios_iguales(usuario_t *a, usuario_t *b, bool check_feed, bool check_seguidores, bool check_seguidos, bool check_bloqueados);
bool feeds_iguales(feed_t *a, feed_t *b);
int contar_publicaciones(feed_t *lista);
usuario_t clonar_usuario(usuario_t* original);
feed_t* clonar_feed(feed_t* original);
void** clonar_array_punteros(void** array_original, uint16_t tamanio);
tuit_t *obtener_tuit_en_posicion(feed_t *lista, int pos);
uint8_t isTrendingTopic(tuit_t *tw);
uint8_t isTrendingTopic2(tuit_t *tw);
uint8_t isTrendingTopic3(tuit_t *tw);
int tamanio_array_tuits(tuit_t** array_tuits);
bool array_tuits_contiene_tuit(tuit_t** array_tuits, tuit_t* tuit);

#endif
