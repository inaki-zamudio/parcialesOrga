#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define WITH_ABI_MESSAGE
#include "../ejs.h"
#include "../utils.h"

TEST(test_ej3_trending_basico) {
  usuario_t user = crear_usuario(1);
  // Feed: id_autor 2, 3, 4, 5
  tuit_t *t0 = crear_tuit("a", 0, 0, 1);
  tuit_t *t1 = crear_tuit("b", 4, 0, 1);
  tuit_t *t2 = crear_tuit("c", 0, 0, 1);
  tuit_t *t3 = crear_tuit("d", 4, 0, 1);

  tuit_t *t0copy = crear_tuit("a", 0, 0, 1);
  tuit_t *t1copy = crear_tuit("b", 4, 0, 1);
  tuit_t *t2copy = crear_tuit("c", 0, 0, 1);
  tuit_t *t3copy = crear_tuit("d", 4, 0, 1);

  agregar_publicacion(&user, crear_publicacion(t0));
  agregar_publicacion(&user, crear_publicacion(t1));
  agregar_publicacion(&user, crear_publicacion(t2));
  agregar_publicacion(&user, crear_publicacion(t3));

  usuario_t user_copy = clonar_usuario(&user);

  tuit_t **arr = TEST_CALL_S(trendingTopic, &user, isTrendingTopic);

  TEST_ASSERT_EQUALS(int, 2, tamanio_array_tuits(arr));
  TEST_ASSERT(array_tuits_contiene_tuit(arr, t3));
  TEST_ASSERT(array_tuits_contiene_tuit(arr, t1));

  TEST_ASSERT(usuarios_iguales(&user, &user_copy, true, true, true, true));

  TEST_ASSERT(tuits_iguales(t0, t0copy));
  TEST_ASSERT(tuits_iguales(t1, t1copy));
  TEST_ASSERT(tuits_iguales(t2, t2copy));
  TEST_ASSERT(tuits_iguales(t3, t3copy));

  liberar_tuit(t0);
  liberar_tuit(t1);
  liberar_tuit(t2);
  liberar_tuit(t3);

  liberar_tuit(t0copy);
  liberar_tuit(t1copy);
  liberar_tuit(t2copy);
  liberar_tuit(t3copy);

  free(arr);
  liberar_usuario(&user);
  liberar_usuario(&user_copy);
}

TEST(test_ej3_trending_sin_match) {
  usuario_t user = crear_usuario(1);

  tuit_t *t0 = crear_tuit("a", 4, 0, 1);
  tuit_t *t1 = crear_tuit("b", 4, 0, 1);
  tuit_t *t2 = crear_tuit("c", 4, 0, 1);
  tuit_t *t3 = crear_tuit("d", 4, 0, 1);

  tuit_t *t0copy = crear_tuit("a", 4, 0, 1);
  tuit_t *t1copy = crear_tuit("b", 4, 0, 1);
  tuit_t *t2copy = crear_tuit("c", 4, 0, 1);
  tuit_t *t3copy = crear_tuit("d", 4, 0, 1);
  
  agregar_publicacion(&user, crear_publicacion(t0));
  agregar_publicacion(&user, crear_publicacion(t1));
  agregar_publicacion(&user, crear_publicacion(t2));
  agregar_publicacion(&user, crear_publicacion(t3));

  usuario_t user_copy = clonar_usuario(&user);

  tuit_t **arr = TEST_CALL_S(trendingTopic, &user, isTrendingTopic2);
  
  TEST_ASSERT_EQUALS(int, 0, tamanio_array_tuits(arr));
  TEST_ASSERT(arr == NULL);

  TEST_ASSERT(usuarios_iguales(&user, &user_copy, true, true, true, true));

  TEST_ASSERT(tuits_iguales(t0, t0copy));
  TEST_ASSERT(tuits_iguales(t1, t1copy));
  TEST_ASSERT(tuits_iguales(t2, t2copy));
  TEST_ASSERT(tuits_iguales(t3, t3copy));

  liberar_tuit(t0);
  liberar_tuit(t1);
  liberar_tuit(t2);
  liberar_tuit(t3);

  liberar_tuit(t0copy);
  liberar_tuit(t1copy);
  liberar_tuit(t2copy);
  liberar_tuit(t3copy);

  free(arr);
  liberar_usuario(&user);
  liberar_usuario(&user_copy);
}


TEST(test_ej3_trending_sin_match_por_autor) {
  usuario_t user = crear_usuario(1);
  tuit_t *t0 = crear_tuit("a", 4, 0, 2);
  tuit_t *t1 = crear_tuit("b", 4, 0, 3);
  tuit_t *t2 = crear_tuit("c", 4, 0, 4);
  tuit_t *t3 = crear_tuit("d", 4, 0, 5);

  tuit_t *t0copy = crear_tuit("a", 4, 0, 2);
  tuit_t *t1copy = crear_tuit("b", 4, 0, 3);
  tuit_t *t2copy = crear_tuit("c", 4, 0, 4);
  tuit_t *t3copy = crear_tuit("d", 4, 0, 5);
  
  agregar_publicacion(&user, crear_publicacion(t0));
  agregar_publicacion(&user, crear_publicacion(t1));
  agregar_publicacion(&user, crear_publicacion(t2));
  agregar_publicacion(&user, crear_publicacion(t3));

  usuario_t user_copy = clonar_usuario(&user);

  tuit_t **arr = TEST_CALL_S(trendingTopic, &user, isTrendingTopic);
  TEST_ASSERT_EQUALS(int, 0, tamanio_array_tuits(arr));
  TEST_ASSERT(arr == NULL);

  TEST_ASSERT(usuarios_iguales(&user, &user_copy, true, true, true, true));

  TEST_ASSERT(tuits_iguales(t0, t0copy));
  TEST_ASSERT(tuits_iguales(t1, t1copy));
  TEST_ASSERT(tuits_iguales(t2, t2copy));
  TEST_ASSERT(tuits_iguales(t3, t3copy));

  liberar_tuit(t0);
  liberar_tuit(t1);
  liberar_tuit(t2);
  liberar_tuit(t3);

  liberar_tuit(t0copy);
  liberar_tuit(t1copy);
  liberar_tuit(t2copy);
  liberar_tuit(t3copy);

  liberar_usuario(&user);
  liberar_usuario(&user_copy);
}

TEST(test_ej3_trending_todos_match) {
  usuario_t user = crear_usuario(3);
  tuit_t *t0 = crear_tuit("m1", 8, 0, 3);
  tuit_t *t1 = crear_tuit("m2", 8, 0, 3);
  tuit_t *t2 = crear_tuit("m3", 8, 0, 3);

  tuit_t *t0copy = crear_tuit("m1", 8, 0, 3);
  tuit_t *t1copy = crear_tuit("m2", 8, 0, 3);
  tuit_t *t2copy = crear_tuit("m3", 8, 0, 3);

  agregar_publicacion(&user, crear_publicacion(t0));
  agregar_publicacion(&user, crear_publicacion(t1));
  agregar_publicacion(&user, crear_publicacion(t2));

  usuario_t user_copy = clonar_usuario(&user);

  tuit_t **arr = TEST_CALL_S(trendingTopic, &user, isTrendingTopic2);
  TEST_ASSERT_EQUALS(int, 3, tamanio_array_tuits(arr));
  TEST_ASSERT(array_tuits_contiene_tuit(arr, t0));
  TEST_ASSERT(array_tuits_contiene_tuit(arr, t1));
  TEST_ASSERT(array_tuits_contiene_tuit(arr, t2));
  
  TEST_ASSERT(usuarios_iguales(&user, &user_copy, true, true, true, true));

  TEST_ASSERT(tuits_iguales(t0, t0copy));
  TEST_ASSERT(tuits_iguales(t1, t1copy));
  TEST_ASSERT(tuits_iguales(t2, t2copy));

  liberar_tuit(t0copy);
  liberar_tuit(t1copy);
  liberar_tuit(t2copy);

  liberar_tuit(t0);
  liberar_tuit(t1);
  liberar_tuit(t2);

  free(arr);
  liberar_usuario(&user);
  liberar_usuario(&user_copy);
}

TEST(test_ej3_trending_grandes_ids) {
  usuario_t user = crear_usuario(4000000000u);

  tuit_t *t0 = crear_tuit("g1", 6, 0, 4000000000u);
  tuit_t *t1 = crear_tuit("g2", 0, 0, 4000000000u);
  agregar_publicacion(&user, crear_publicacion(t0));
  agregar_publicacion(&user, crear_publicacion(t1));

  usuario_t user_copy = clonar_usuario(&user);

  tuit_t **arr = TEST_CALL_S(trendingTopic, &user, isTrendingTopic);
  TEST_ASSERT_EQUALS(int, 1, tamanio_array_tuits(arr));
  TEST_ASSERT(array_tuits_contiene_tuit(arr, t0));

  TEST_ASSERT(usuarios_iguales(&user, &user_copy, true, true, true, true));

  liberar_tuit(t0);
  liberar_tuit(t1);
  free(arr);
  liberar_usuario(&user);
  liberar_usuario(&user_copy);
}

TEST(test_ej3_trending_llamadas_multiples) {
  usuario_t user = crear_usuario(14);

  tuit_t *t0 = crear_tuit("m1", 0, 0, 2);
  tuit_t *t1 = crear_tuit("m2", 6, 0, 14);
  agregar_publicacion(&user, crear_publicacion(t0));
  agregar_publicacion(&user, crear_publicacion(t1));

  usuario_t user_copy = clonar_usuario(&user);

  tuit_t **a = TEST_CALL_S(trendingTopic, &user,
                           isTrendingTopic); 
  TEST_ASSERT_EQUALS(int, 1, tamanio_array_tuits(a));
  TEST_ASSERT(array_tuits_contiene_tuit(a, t1));
  
  tuit_t *t2 = crear_tuit("m3", 8, 0, 14);
  
  agregar_publicacion(&user, crear_publicacion(t2));
  agregar_publicacion(&user_copy, crear_publicacion(t2));
  
  tuit_t **b = TEST_CALL_S(trendingTopic, &user, isTrendingTopic2);
  TEST_ASSERT_EQUALS(int, 2, tamanio_array_tuits(b));
  TEST_ASSERT(array_tuits_contiene_tuit(b, t1));
  TEST_ASSERT(array_tuits_contiene_tuit(b, t2));

  TEST_ASSERT(usuarios_iguales(&user, &user_copy, true, true, true, true));
  
  liberar_tuit(t0);
  liberar_tuit(t1);
  liberar_tuit(t2);
  free(a);
  free(b);
  liberar_usuario(&user);
  liberar_usuario(&user_copy);
}

TEST(test_ej3_trending_feed_vacio) {
  usuario_t user = crear_usuario(100);
  usuario_t user_copy = clonar_usuario(&user);
  tuit_t **arr = TEST_CALL_S(trendingTopic, &user, isTrendingTopic);
  TEST_ASSERT_EQUALS(int, 0, tamanio_array_tuits(arr));
  TEST_ASSERT(arr == NULL);
  liberar_usuario(&user);
  liberar_usuario(&user_copy);
}


int main(int argc, char *argv[]) {
  printf("Corriendo los tests del ejercicio 3...\n");
  test_ej3_trending_basico();
  test_ej3_trending_sin_match();
  test_ej3_trending_sin_match_por_autor();
  test_ej3_trending_todos_match();
  test_ej3_trending_grandes_ids();
  test_ej3_trending_llamadas_multiples();
  test_ej3_trending_feed_vacio();
  tests_end("Ejercicio 3");
  return 0;
}
