#include "../ejs.h"
#include <string.h>

// Auxiliar: agregar al feed
void agregar_al_feed(tuit_t* tuit, feed_t* feed) {
  publicacion_t* publicacion = malloc(sizeof(publicacion_t));
  publicacion->next = feed->first;
  publicacion->value = tuit;
  feed->first = publicacion;
}






// Función principal: publicar un tuit
tuit_t *publicar(char *mensaje, usuario_t *user) {

  // Armo el tuit:
  tuit_t* res = malloc(sizeof(tuit_t));
  strcpy(res->mensaje, mensaje);
  res->favoritos = 0;
  res->retuits = 0;
  res->id_autor = user->id;

  // Agregarlo al principio de su feed
  feed_t* feed = user->feed;
  agregar_al_feed(res, feed);

  // Agregarlo al feed de todos sus seguidores
  usuario_t** followers = user->seguidores;
  for (uint32_t i = 0; i < user->cantSeguidores; i++) {
    usuario_t* seguidor = followers[i];
    feed_t* feed_seguidor = seguidor->feed;
    agregar_al_feed(res, feed_seguidor);
  }

  return res;
}


