#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#define WITH_ABI_MESSAGE
#include "../ejs.h"
#include "../utils.h"

TEST(test_ej1_publicar_sin_seguidores) {
  usuario_t user = crear_usuario(42);
  usuario_t user_copy = clonar_usuario(&user);
  char *mensaje = "Mi primer tuit";

  tuit_t *result1 = TEST_CALL_S(publicar, mensaje, &user);

  TEST_ASSERT(result1 != NULL);
  TEST_ASSERT_EQUALS(uint32_t, 42, result1->id_autor);
  TEST_ASSERT(strcmp(result1->mensaje, mensaje) == 0);
  TEST_ASSERT_EQUALS(uint32_t, 0, result1->favoritos);
  TEST_ASSERT_EQUALS(uint32_t, 0, result1->retuits);
  TEST_ASSERT(user.feed != NULL);
  TEST_ASSERT(user.feed->first != NULL);
  TEST_ASSERT(result1 == user.feed->first->value);

  TEST_ASSERT(usuarios_iguales(&user, &user_copy, false, true, true, true));

  free(result1);
  liberar_usuario(&user);
  liberar_usuario(&user_copy);
}

TEST(test_ej1_publicar_con_seguidor) {
  usuario_t autor = crear_usuario(100);
  
  usuario_t seg1 = crear_usuario(200);
  
  autor.seguidores = (usuario_t **)malloc(1 * sizeof(usuario_t*));
  autor.seguidores[0] = &seg1;
  autor.cantSeguidores++;

  usuario_t autor_copy = clonar_usuario(&autor);
  usuario_t seg1_copy = clonar_usuario(&seg1);

  char *mensaje = "Nuevo tuit del autor";
  tuit_t *result2 = TEST_CALL_S(publicar, mensaje, &autor);

  TEST_ASSERT(result2 != NULL);
  TEST_ASSERT_EQUALS(uint32_t, autor.id, result2->id_autor);
  TEST_ASSERT(strcmp(result2->mensaje, mensaje) == 0);
  TEST_ASSERT(autor.seguidores[0]->feed != NULL);
  TEST_ASSERT(autor.seguidores[0]->feed->first != NULL);
  TEST_ASSERT(result2 == autor.seguidores[0]->feed->first->value);

  TEST_ASSERT(usuarios_iguales(&autor, &autor_copy, false, true, true, true));
  TEST_ASSERT(usuarios_iguales(&seg1, &seg1_copy, false, true, true, true));

  free(result2);

  liberar_usuario(&seg1);
  liberar_usuario(&seg1_copy);
  liberar_usuario(&autor);
  liberar_usuario(&autor_copy);
}

TEST(test_ej1_publicar_clonado_mensaje) {
  usuario_t user = crear_usuario(123);
  char mensaje[] = "Mensaje original";

  usuario_t user_copy = clonar_usuario(&user);

  tuit_t *result3 = TEST_CALL_S(publicar, mensaje, &user);

  TEST_ASSERT(result3 != NULL);

  mensaje[0] = 'X';

  TEST_ASSERT_EQUALS(uint32_t, user.id, result3->id_autor);
  TEST_ASSERT(strcmp(result3->mensaje, "Mensaje original") == 0);
  TEST_ASSERT(user.feed != NULL);
  TEST_ASSERT(user.feed->first != NULL);
  TEST_ASSERT(result3 == user.feed->first->value);

  TEST_ASSERT(usuarios_iguales(&user, &user_copy, false, true, true, true));

  free(result3);
  liberar_usuario(&user);
  liberar_usuario(&user_copy);
}

TEST(test_ej1_publicar_dos_seguidores) {
  usuario_t autor = crear_usuario(10);
  usuario_t seg1 = crear_usuario(20);
  usuario_t seg2 = crear_usuario(30);

  autor.seguidores = (usuario_t **)malloc(2 * sizeof(usuario_t*));
  autor.seguidores[0] = &seg1;
  autor.seguidores[1] = &seg2;
  autor.cantSeguidores = 2;

  char *mensaje = "Hola a todos";

  usuario_t autor_copy = clonar_usuario(&autor);
  usuario_t seg1_copy = clonar_usuario(&seg1);
  usuario_t seg2_copy = clonar_usuario(&seg2);

  tuit_t *result4 = TEST_CALL_S(publicar, mensaje, &autor);

  TEST_ASSERT_EQUALS(uint32_t, 10, result4->id_autor);
  TEST_ASSERT(autor.feed->first != NULL);
  TEST_ASSERT(autor.feed != NULL);
  TEST_ASSERT(autor.feed->first != NULL);
  TEST_ASSERT(result4 == autor.feed->first->value);
  TEST_ASSERT(seg1.feed->first != NULL);
  TEST_ASSERT(seg1.feed != NULL);
  TEST_ASSERT(seg1.feed->first != NULL);
  TEST_ASSERT(result4 == seg1.feed->first->value);
  TEST_ASSERT(seg2.feed != NULL);
  TEST_ASSERT(seg2.feed->first != NULL);
  TEST_ASSERT(result4 == seg2.feed->first->value);

  TEST_ASSERT(usuarios_iguales(&autor, &autor_copy, false, true, true, true));
  TEST_ASSERT(usuarios_iguales(&seg1, &seg1_copy, false, true, true, true));
  TEST_ASSERT(usuarios_iguales(&seg2, &seg2_copy, false, true, true, true));


  free(result4);
  liberar_usuario(&seg1);
  liberar_usuario(&seg2);
  liberar_usuario(&autor);
  liberar_usuario(&seg1_copy);
  liberar_usuario(&seg2_copy);
  liberar_usuario(&autor_copy);
}

TEST(test_ej1_publicar_inserta_al_principio) {
  usuario_t autor = crear_usuario(1);

  usuario_t seg1 = crear_usuario(2);

  autor.seguidores = (usuario_t **)malloc(1 * sizeof(usuario_t*));
  autor.seguidores[0] = &seg1;
  autor.cantSeguidores = 1;

  tuit_t *t = crear_tuit("viejo", 0, 0, 999);

  agregar_publicacion(&seg1, crear_publicacion(t));

  usuario_t autor_copy = clonar_usuario(&autor);
  usuario_t seg1_copy = clonar_usuario(&seg1);

  tuit_t *result5 = TEST_CALL_S(publicar, "nuevo", &autor);
  
  TEST_ASSERT(autor.feed->first != NULL);
  TEST_ASSERT(autor.feed != NULL);
  TEST_ASSERT(autor.feed->first != NULL);
  TEST_ASSERT(result5 == autor.feed->first->value);
  TEST_ASSERT(seg1.feed->first != NULL);
  TEST_ASSERT(seg1.feed != NULL);
  TEST_ASSERT(seg1.feed->first != NULL);
  TEST_ASSERT(result5 == seg1.feed->first->value);
  TEST_ASSERT(strcmp(result5->mensaje, "nuevo") == 0);

  TEST_ASSERT(usuarios_iguales(&autor, &autor_copy, false, true, true, true));
  TEST_ASSERT(usuarios_iguales(&seg1, &seg1_copy, false, true, true, true));

  free(t);
  free(result5);
  liberar_usuario(&seg1);
  liberar_usuario(&autor);
  liberar_usuario(&seg1_copy);
  liberar_usuario(&autor_copy);
}

TEST(test_ej1_publicar_doble_orden) {
  usuario_t autor = crear_usuario(7);
  usuario_t seg1 = crear_usuario(8);

  autor.seguidores = (usuario_t **)malloc(1 * sizeof(usuario_t*));
  autor.seguidores[0] = &seg1;
  autor.cantSeguidores = 1;

  usuario_t autor_copy = clonar_usuario(&autor);
  usuario_t seg1_copy = clonar_usuario(&seg1);


  tuit_t *t1 = TEST_CALL_S(publicar, "uno", &autor);
  tuit_t *t2 = TEST_CALL_S(publicar, "dos", &autor);

  TEST_ASSERT(autor.feed->first != NULL);
  TEST_ASSERT(autor.feed != NULL);
  TEST_ASSERT(autor.feed->first != NULL);
  TEST_ASSERT(t2 == autor.feed->first->value);
  TEST_ASSERT(autor.feed->first->next != NULL);
  TEST_ASSERT(t1 == autor.feed->first->next->value);
  TEST_ASSERT(seg1.feed->first != NULL);
  TEST_ASSERT(seg1.feed != NULL);
  TEST_ASSERT(seg1.feed->first != NULL);
  TEST_ASSERT(t2 == seg1.feed->first->value);
  TEST_ASSERT(seg1.feed->first->next != NULL);
  TEST_ASSERT(t1 == seg1.feed->first->next->value);
  TEST_ASSERT(strcmp(t2->mensaje, "dos") == 0);
  TEST_ASSERT(strcmp(t1->mensaje, "uno") == 0);

  TEST_ASSERT(usuarios_iguales(&autor, &autor_copy, false, true, true, true));
  TEST_ASSERT(usuarios_iguales(&seg1, &seg1_copy, false, true, true, true));

  free(t1);
  free(t2);

  liberar_usuario(&seg1);
  liberar_usuario(&autor);
  liberar_usuario(&seg1_copy);
  liberar_usuario(&autor_copy);
}

TEST(test_ej1_publicar_dos_autores_mismo_seguidor) {
  usuario_t a1 = crear_usuario(91);
  usuario_t a2 = crear_usuario(92);

  usuario_t seg = crear_usuario(93);
  seg.seguidores = NULL;
  seg.seguidos = (usuario_t **)malloc(2 * sizeof(usuario_t*));
  seg.bloqueados = NULL;

  a1.seguidores = (usuario_t **)malloc(1 * sizeof(usuario_t*));
  a2.seguidores = (usuario_t **)malloc(1 * sizeof(usuario_t*));
  a1.seguidores[0] = &seg;
  a2.seguidores[0] = &seg;
  a1.cantSeguidores = 1;
  a2.cantSeguidores = 1;

  usuario_t a1_copy = clonar_usuario(&a1);
  usuario_t a2_copy = clonar_usuario(&a2);
  usuario_t seg_copy = clonar_usuario(&seg);

  tuit_t *t1 = TEST_CALL_S(publicar, "A", &a1);
  tuit_t *t2 = TEST_CALL_S(publicar, "B", &a2);

  TEST_ASSERT(seg.feed->first != NULL);
  TEST_ASSERT(seg.feed != NULL);
  TEST_ASSERT(seg.feed->first != NULL);
  TEST_ASSERT(t2 == seg.feed->first->value);
  TEST_ASSERT(seg.feed->first->next != NULL);
  TEST_ASSERT(t1 == seg.feed->first->next->value);
  TEST_ASSERT(strcmp(t2->mensaje, "B") == 0);
  TEST_ASSERT(strcmp(t1->mensaje, "A") == 0);

  TEST_ASSERT(usuarios_iguales(&a1, &a1_copy, false, true, true, true));
  TEST_ASSERT(usuarios_iguales(&a2, &a2_copy, false, true, true, true));
  TEST_ASSERT(usuarios_iguales(&seg, &seg_copy, false, true, true, true));

  free(t1);
  free(t2);
  liberar_usuario(&seg);
  liberar_usuario(&a1);
  liberar_usuario(&a2);
  liberar_usuario(&seg_copy);
  liberar_usuario(&a1_copy);
  liberar_usuario(&a2_copy);
}

int main(int argc, char *argv[]) {
  printf("Corriendo los tests del ejercicio 1...\n");

  test_ej1_publicar_sin_seguidores();
  test_ej1_publicar_con_seguidor();
  test_ej1_publicar_clonado_mensaje();
  test_ej1_publicar_dos_seguidores();
  test_ej1_publicar_inserta_al_principio();
  test_ej1_publicar_doble_orden();
  test_ej1_publicar_dos_autores_mismo_seguidor();

  tests_end("Ejercicio 1");
}
