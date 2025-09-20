#include "utils.h"
#include "ejs.h"

tuit_t *crear_tuit(char *mensaje, uint16_t favoritos, uint16_t retuits,
                   uint32_t id_autor) {
  tuit_t *t = (tuit_t *)malloc(sizeof(tuit_t));
  if (t == NULL)
    return NULL;

  t->favoritos = favoritos;
  t->retuits = retuits;
  t->id_autor = id_autor;

  strcpy(t->mensaje, mensaje);

  return t;
}

usuario_t crear_usuario(uint32_t id) {
  usuario_t user;

  user.id = id;
  user.feed = (feed_t*)malloc(sizeof(feed_t));
  user.feed->first = NULL;
  user.seguidores = NULL;
  user.seguidos = NULL;
  user.bloqueados = NULL;

  user.cantBloqueados = 0;
  user.cantSeguidores = 0;
  user.cantSeguidos = 0;

  return user;
}

usuario_t clonar_usuario(usuario_t* original) {
  usuario_t clon;

  clon.id = original->id;
  clon.feed = clonar_feed(original->feed);
  clon.seguidores =(usuario_t**) clonar_array_punteros((void**)original->seguidores, original->cantSeguidores);
  clon.seguidos =(usuario_t**) clonar_array_punteros((void**)original->seguidos, original->cantSeguidos);
  clon.bloqueados =(usuario_t**) clonar_array_punteros((void**)original->bloqueados, original->cantBloqueados);

  clon.cantBloqueados = original->cantBloqueados;
  clon.cantSeguidores = original->cantSeguidores;
  clon.cantSeguidos = original->cantSeguidos;

  return clon;
}

feed_t* clonar_feed(feed_t* original){
  feed_t *nuevo = (feed_t*)malloc(sizeof(feed_t));
  nuevo->first = NULL;
  
  publicacion_t *current = original->first;
  while (current != NULL){
    agregar_publicacion_al_final(nuevo, crear_publicacion(current->value));  
    current = current->next;  
  }

  return nuevo;
}

void** clonar_array_punteros(void** array_original, uint16_t tamanio){
  void** array_clon = (void**)malloc(tamanio*sizeof(void*));
  for(uint16_t i = 0 ; i<tamanio; i++){
    array_clon[i]=array_original[i];
  }
  return array_clon;
}

publicacion_t *crear_publicacion(tuit_t *tuit_value) {
  publicacion_t *publicacion = (publicacion_t *)malloc(sizeof(publicacion_t));
  if (publicacion == NULL)
    return NULL;

  publicacion->value = tuit_value;
  publicacion->next = NULL;

  return publicacion;
}

void agregar_publicacion(usuario_t *user, publicacion_t *publicacion) {
  if (user->feed == NULL) {
    user->feed = (feed_t *)malloc(sizeof(feed_t));
    user->feed->first = NULL;
  }

  if (user->feed->first != NULL) {
    publicacion->next = user->feed->first;
  }

  user->feed->first = publicacion;
}

void agregar_publicacion_al_final(feed_t *feed, publicacion_t *publicacion) {
  publicacion_t* current = feed->first;
  if (current == NULL){
    feed->first = publicacion;
  } else {
    while(current->next != NULL){
      current = current->next;
    }
    current->next = publicacion;
  }
}

void liberar_tuit(tuit_t *tuit){
  if (tuit == NULL) {
    return;
  }
  free(tuit);
}

void liberar_publicacion(publicacion_t *publicacion) {
  if (publicacion == NULL)
    return;

  free(publicacion);
}

void liberar_feed(feed_t *lista) {
  if (lista->first == NULL) {
    free(lista);
    return;
  }
  publicacion_t *p = lista->first;
  while (p != NULL) {
    publicacion_t *r = p->next;
    free(p);
    p = r;
  }

  free(lista);
}

void liberar_usuario(usuario_t *user) {
  if (user->feed != NULL) {
    liberar_feed(user->feed);
  }

  if (user->seguidores != NULL) {
    free(user->seguidores);
  }

  if (user->bloqueados != NULL) {
    free(user->bloqueados);
  }

  if (user->seguidos != NULL) {
    free(user->seguidos);
  }
}

bool tuits_iguales(tuit_t *a, tuit_t *b) {
  if (a == NULL && b == NULL)
    return true;
  if (a == NULL || b == NULL)
    return false;

  if (a->favoritos != b->favoritos || a->retuits != b->retuits ||
      a->id_autor != b->id_autor) {
    return false;
  }

  if (strcmp(a->mensaje, b->mensaje) != 0)
    return false;

  return true;
}

bool usuarios_iguales(usuario_t *a, usuario_t *b, bool check_feed, bool check_seguidores, bool check_seguidos, bool check_bloqueados) {
  if (a == NULL && b == NULL)
    return true;
  if (a == NULL || b == NULL)
    return false;

  bool ret = true;
  if(check_seguidores){
    if(a->cantSeguidores != b->cantSeguidores)
      return false;
    
    for (uint32_t i = 0 ; i < a->cantSeguidores ; i++){
      ret &= (a->seguidores[i]==b->seguidores[i]);
    }
    if (!ret)
      return false;
  }

  if(check_bloqueados){
    if(a->cantBloqueados != b->cantBloqueados)
      return false;

    for (uint32_t i = 0 ; i < a->cantBloqueados ; i++){
      ret &= (a->bloqueados[i] == b->bloqueados[i]);
    }
    if (!ret)
      return false;
  }

  if(check_seguidos){
    if(a->cantSeguidos != b->cantSeguidos)
      return false;

    for (uint32_t i = 0 ; i < a->cantSeguidos ; i++){
      ret &= (a->seguidos[i] == b->seguidos[i]);
    }
    if (!ret)
      return false;
  }

  if(check_feed){
    if (!feeds_iguales(a->feed, b->feed))
      return false;
  }

  return (a->id == b->id);
}

int contar_publicaciones(feed_t *lista) {
  if (lista == NULL)
    return 0;

  int count = 0;
  publicacion_t *current = lista->first;
  while (current != NULL) {
    count++;
    current = current->next;
  }
  return count;
}

bool feeds_iguales(feed_t *a, feed_t *b){
  if(a == NULL && b == NULL)
    return true;
  if(a == NULL || b == NULL)
    return false;

  publicacion_t *currentA = a->first;
  publicacion_t *currentB = b->first;
  while(currentA != NULL && currentB != NULL){
    
    if(currentA->value->id_autor != currentB->value->id_autor || strcmp(currentA->value->mensaje, currentB->value->mensaje) != 0){
      return false;
    }

    currentA = currentA->next;
    currentB = currentB->next;
  }
  if(currentA == NULL && currentB == NULL){
    return true;
  }else {
    return false;
  }
}

tuit_t *obtener_tuit_en_posicion(feed_t *lista, int pos) {
  if (lista == NULL || pos < 0)
    return NULL;

  publicacion_t *current = lista->first;
  int index = 0;

  while (current != NULL && index < pos) {
    current = current->next;
    index++;
  }

  return (current != NULL) ? current->value : NULL;
}

uint8_t isTrendingTopic(tuit_t *tw) {
  if (tw == NULL)
    return 0;
  return (uint8_t)(tw->favoritos > 3);
}

uint8_t isTrendingTopic2(tuit_t *tw) {
  if (tw == NULL)
    return 0;
  return (uint8_t)(tw->favoritos > 5);
}

uint8_t isTrendingTopic3(tuit_t *tw) {
  if (tw == NULL)
    return 0;
  return (uint8_t)(tw->favoritos > 7);
}

int tamanio_array_tuits(tuit_t** array_tuits){
  if(array_tuits != NULL){
    int i = 0;
    while (array_tuits[i] != NULL){
      i++;
    }
    return i;
  }else{
    return 0;
  }
}

bool array_tuits_contiene_tuit(tuit_t** array_tuits, tuit_t* tuit){
  if(array_tuits != NULL){    
    int i = 0;      
    while (array_tuits[i] != NULL){
      if(array_tuits[i] == tuit){
        return true;
      }
      i++;
    }
    return false;     
  }else{
    return false;
  }
}
