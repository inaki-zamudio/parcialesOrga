#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define WITH_ABI_MESSAGE
#include "../ejs.h"
#include "../utils.h"

TEST(test_ej2_bloquear_basico) {
  usuario_t user = crear_usuario(10);
  usuario_t a = crear_usuario(100);
  usuario_t b = crear_usuario(200);

  // Feed de user: a -> b -> a
  tuit_t *t1 = crear_tuit("hola", 0, 0, a.id);
  tuit_t *t2 = crear_tuit("chau", 0, 0, b.id);
  tuit_t *t3 = crear_tuit("otra", 0, 0, a.id);
  tuit_t *t4 = crear_tuit("buenas", 0, 0, user.id);
  tuit_t *t5 = crear_tuit("buenas buenas", 0, 0, b.id);

  user.bloqueados = (usuario_t **)malloc(1 * sizeof(usuario_t*));

  agregar_publicacion(&user, crear_publicacion(t1));
  agregar_publicacion(&user, crear_publicacion(t2));
  agregar_publicacion(&user, crear_publicacion(t3));
  agregar_publicacion(&a, crear_publicacion(t4));
  agregar_publicacion(&a, crear_publicacion(t5));

  usuario_t user_copy = clonar_usuario(&user);
  usuario_t a_copy = clonar_usuario(&a);
  usuario_t b_copy = clonar_usuario(&b);

  // Bloqueo a 'a' y debe borrar t1 y t3. Y t4 de a
  TEST_CALL_V(bloquearUsuario, &user, &a);

  // Queda solo el de 'b'
  TEST_ASSERT_EQUALS(int32_t, 1, contar_publicaciones(user.feed));
  TEST_ASSERT_EQUALS(int32_t, 1, contar_publicaciones(a.feed));

  tuit_t *rem = obtener_tuit_en_posicion(user.feed, 0);
  TEST_ASSERT(rem != NULL);
  TEST_ASSERT_EQUALS(uint32_t, b.id, rem->id_autor);
  TEST_ASSERT(strcmp("chau", rem->mensaje) == 0);
  TEST_ASSERT(rem == t2);

  rem = obtener_tuit_en_posicion(a.feed, 0);
  TEST_ASSERT(rem != NULL);
  TEST_ASSERT_EQUALS(uint32_t, b.id, rem->id_autor);
  TEST_ASSERT(strcmp("buenas buenas", rem->mensaje) == 0);
  TEST_ASSERT(rem == t5);

  // bloqueados: a debe estar último
  TEST_ASSERT(user.cantBloqueados = user_copy.cantBloqueados + 1);
  TEST_ASSERT(user.bloqueados[user.cantBloqueados - 1] == &a);

  // No se debe haber modificado nada más
  TEST_ASSERT(usuarios_iguales(&user, &user_copy, false, true, true, false));
  TEST_ASSERT(usuarios_iguales(&a, &a_copy, false, true, true, true));
  TEST_ASSERT(usuarios_iguales(&b, &b_copy, true, true, true, true));

  liberar_tuit(t1);
  liberar_tuit(t2);
  liberar_tuit(t3);
  liberar_tuit(t4);
  liberar_tuit(t5);
  liberar_usuario(&user);
  liberar_usuario(&b);
  liberar_usuario(&a);
  liberar_usuario(&user_copy);
  liberar_usuario(&b_copy);
  liberar_usuario(&a_copy);
}

TEST(test_ej2_bloquear_sin_coincidencias) {
  usuario_t user = crear_usuario(10);
  usuario_t a = crear_usuario(100);
  usuario_t b = crear_usuario(200);


  tuit_t *t2 = crear_tuit("chau", 0, 0, b.id);
  tuit_t *t3 = crear_tuit("otra", 0, 0, b.id);
  tuit_t *t4 = crear_tuit("buenas", 0, 0, b.id);
  tuit_t *t5 = crear_tuit("buenas buenas", 0, 0, b.id);

  tuit_t *t2_copy = crear_tuit("chau", 0, 0, b.id);
  tuit_t *t3_copy = crear_tuit("otra", 0, 0, b.id);
  tuit_t *t4_copy = crear_tuit("buenas", 0, 0, b.id);
  tuit_t *t5_copy = crear_tuit("buenas buenas", 0, 0, b.id);

  user.bloqueados = (usuario_t **)malloc(1 * sizeof(usuario_t*));

  agregar_publicacion(&user, crear_publicacion(t2));
  agregar_publicacion(&user, crear_publicacion(t3));
  agregar_publicacion(&a, crear_publicacion(t4));
  agregar_publicacion(&a, crear_publicacion(t5));

  usuario_t user_copy = clonar_usuario(&user);
  usuario_t a_copy = clonar_usuario(&a);
  usuario_t b_copy = clonar_usuario(&b);

  TEST_CALL_V(bloquearUsuario, &user, &a);

  TEST_ASSERT_EQUALS(int32_t, 2, contar_publicaciones(user.feed));
  TEST_ASSERT_EQUALS(int32_t, 2, contar_publicaciones(a.feed));

  // bloqueados: a debe estar último
  TEST_ASSERT(user.cantBloqueados = user_copy.cantBloqueados + 1);
  TEST_ASSERT(user.bloqueados[user.cantBloqueados - 1] == &a);

  // No se debe haber modificado nada más
  TEST_ASSERT(usuarios_iguales(&user, &user_copy, true, true, true, false));
  TEST_ASSERT(usuarios_iguales(&a, &a_copy, true, true, true, true));
  TEST_ASSERT(usuarios_iguales(&b, &b_copy, true, true, true, true));

  TEST_ASSERT(tuits_iguales(t2, t2_copy));
  TEST_ASSERT(tuits_iguales(t3, t3_copy));
  TEST_ASSERT(tuits_iguales(t4, t4_copy));
  TEST_ASSERT(tuits_iguales(t5, t5_copy));

  liberar_tuit(t2);
  liberar_tuit(t3);
  liberar_tuit(t4);
  liberar_tuit(t5);
  liberar_tuit(t2_copy);
  liberar_tuit(t3_copy);
  liberar_tuit(t4_copy);
  liberar_tuit(t5_copy);

  liberar_usuario(&user);
  liberar_usuario(&b);
  liberar_usuario(&a);
  liberar_usuario(&user_copy);
  liberar_usuario(&b_copy);
  liberar_usuario(&a_copy);
}

TEST(test_ej2_bloquear_todas_coincidencias) {
  usuario_t user = crear_usuario(10);
  usuario_t a = crear_usuario(100);

  // Feed de user: a -> b -> a
  tuit_t *t1 = crear_tuit("hola", 0, 0, a.id);
  tuit_t *t2 = crear_tuit("chau", 0, 0, a.id);
  tuit_t *t3 = crear_tuit("otra", 0, 0, a.id);
  tuit_t *t4 = crear_tuit("buenas", 0, 0, user.id);
  tuit_t *t5 = crear_tuit("buenas buenas", 0, 0, user.id);

  tuit_t *t1_copy = crear_tuit("hola", 0, 0, a.id);
  tuit_t *t2_copy = crear_tuit("chau", 0, 0, a.id);
  tuit_t *t3_copy = crear_tuit("otra", 0, 0, a.id);
  tuit_t *t4_copy = crear_tuit("buenas", 0, 0, user.id);
  tuit_t *t5_copy = crear_tuit("buenas buenas", 0, 0, user.id);

  user.bloqueados = (usuario_t **)malloc(1 * sizeof(usuario_t*));

  agregar_publicacion(&user, crear_publicacion(t1));
  agregar_publicacion(&user, crear_publicacion(t2));
  agregar_publicacion(&user, crear_publicacion(t3));
  agregar_publicacion(&a, crear_publicacion(t4));
  agregar_publicacion(&a, crear_publicacion(t5));

  usuario_t user_copy = clonar_usuario(&user);
  usuario_t a_copy = clonar_usuario(&a);

  TEST_CALL_V(bloquearUsuario, &user, &a);

  TEST_ASSERT_EQUALS(int32_t, 0, contar_publicaciones(user.feed));
  TEST_ASSERT_EQUALS(int32_t, 0, contar_publicaciones(a.feed));

  // bloqueados: a debe estar último
  TEST_ASSERT(user.cantBloqueados = user_copy.cantBloqueados + 1);
  TEST_ASSERT(user.bloqueados[user.cantBloqueados - 1] == &a);

  // No se debe haber modificado nada más
  TEST_ASSERT(usuarios_iguales(&user, &user_copy, false, true, true, false));
  TEST_ASSERT(usuarios_iguales(&a, &a_copy, false, true, true, true));

  TEST_ASSERT(tuits_iguales(t2, t2_copy));
  TEST_ASSERT(tuits_iguales(t3, t3_copy));
  TEST_ASSERT(tuits_iguales(t4, t4_copy));
  TEST_ASSERT(tuits_iguales(t5, t5_copy));

  liberar_tuit(t1);
  liberar_tuit(t2);
  liberar_tuit(t3);
  liberar_tuit(t4);
  liberar_tuit(t5);
  liberar_tuit(t1_copy);
  liberar_tuit(t2_copy);
  liberar_tuit(t3_copy);
  liberar_tuit(t4_copy);
  liberar_tuit(t5_copy);

  liberar_usuario(&user);
  liberar_usuario(&a);
  liberar_usuario(&user_copy);
  liberar_usuario(&a_copy);
}

TEST(test_ej2_solo_feed_bloqueado) {
  usuario_t user = crear_usuario(10);
  usuario_t a = crear_usuario(100);
  usuario_t b = crear_usuario(200);

  // Feed de user: a -> b -> a
  tuit_t *t2 = crear_tuit("chau", 0, 0, b.id);
  tuit_t *t4 = crear_tuit("buenas", 0, 0, user.id);
  tuit_t *t5 = crear_tuit("buenas buenas", 0, 0, b.id);

  user.bloqueados = (usuario_t **)malloc(1 * sizeof(usuario_t*));


  agregar_publicacion(&user, crear_publicacion(t2));
  agregar_publicacion(&a, crear_publicacion(t4));
  agregar_publicacion(&a, crear_publicacion(t5));

  usuario_t user_copy = clonar_usuario(&user);
  usuario_t a_copy = clonar_usuario(&a);
  usuario_t b_copy = clonar_usuario(&b);

  tuit_t *t2_copy = crear_tuit("chau", 0, 0, b.id);
  tuit_t *t4_copy = crear_tuit("buenas", 0, 0, user.id);
  tuit_t *t5_copy = crear_tuit("buenas buenas", 0, 0, b.id);

  TEST_CALL_V(bloquearUsuario, &user, &a);

  // Queda solo el de 'b'
  TEST_ASSERT_EQUALS(int32_t, 1, contar_publicaciones(user.feed));
  TEST_ASSERT_EQUALS(int32_t, 1, contar_publicaciones(a.feed));

  tuit_t *rem = obtener_tuit_en_posicion(user.feed, 0);
  TEST_ASSERT(rem != NULL);
  TEST_ASSERT_EQUALS(uint32_t, b.id, rem->id_autor);
  TEST_ASSERT(strcmp("chau", rem->mensaje) == 0);
  TEST_ASSERT(rem == t2);

  rem = obtener_tuit_en_posicion(a.feed, 0);
  TEST_ASSERT(rem != NULL);
  TEST_ASSERT_EQUALS(uint32_t, b.id, rem->id_autor);
  TEST_ASSERT(strcmp("buenas buenas", rem->mensaje) == 0);
  TEST_ASSERT(rem == t5);

  // bloqueados: a debe estar último
  TEST_ASSERT(user.cantBloqueados = user_copy.cantBloqueados + 1);
  TEST_ASSERT(user.bloqueados[user.cantBloqueados - 1] == &a);

  // No se debe haber modificado nada más
  TEST_ASSERT(usuarios_iguales(&user, &user_copy, true, true, true, false));
  TEST_ASSERT(usuarios_iguales(&a, &a_copy, false, true, true, true));
  TEST_ASSERT(usuarios_iguales(&b, &b_copy, true, true, true, true));

  TEST_ASSERT(tuits_iguales(t2, t2_copy));
  TEST_ASSERT(tuits_iguales(t4, t4_copy));
  TEST_ASSERT(tuits_iguales(t5, t5_copy));

  liberar_tuit(t2);
  liberar_tuit(t4);
  liberar_tuit(t5);
  liberar_tuit(t2_copy);
  liberar_tuit(t4_copy);
  liberar_tuit(t5_copy);
  liberar_usuario(&user);
  liberar_usuario(&b);
  liberar_usuario(&a);
  liberar_usuario(&user_copy);
  liberar_usuario(&b_copy);
  liberar_usuario(&a_copy);
}

TEST(test_ej2_bloquear_solo_feed_bloqueador) {
  usuario_t user = crear_usuario(10);
  usuario_t a = crear_usuario(100);
  usuario_t b = crear_usuario(200);

  // Feed de user: a -> b -> a
  tuit_t *t2 = crear_tuit("chau", 0, 0, b.id);
  tuit_t *t4 = crear_tuit("buenas", 0, 0, a.id);
  tuit_t *t5 = crear_tuit("buenas buenas", 0, 0, b.id);

  user.bloqueados = (usuario_t **)malloc(1 * sizeof(usuario_t*));


  agregar_publicacion(&user, crear_publicacion(t2));
  agregar_publicacion(&user, crear_publicacion(t4));
  agregar_publicacion(&a, crear_publicacion(t5));

  usuario_t user_copy = clonar_usuario(&user);
  usuario_t a_copy = clonar_usuario(&a);
  usuario_t b_copy = clonar_usuario(&b);

  tuit_t *t2_copy = crear_tuit("chau", 0, 0, b.id);
  tuit_t *t4_copy = crear_tuit("buenas", 0, 0, a.id);
  tuit_t *t5_copy = crear_tuit("buenas buenas", 0, 0, b.id);

  TEST_CALL_V(bloquearUsuario, &user, &a);

  // Queda solo el de 'b'
  TEST_ASSERT_EQUALS(int32_t, 1, contar_publicaciones(user.feed));
  TEST_ASSERT_EQUALS(int32_t, 1, contar_publicaciones(a.feed));

  tuit_t *rem = obtener_tuit_en_posicion(user.feed, 0);
  TEST_ASSERT(rem != NULL);
  TEST_ASSERT_EQUALS(uint32_t, b.id, rem->id_autor);
  TEST_ASSERT(strcmp("chau", rem->mensaje) == 0);
  TEST_ASSERT(rem == t2);

  rem = obtener_tuit_en_posicion(a.feed, 0);
  TEST_ASSERT(rem != NULL);
  TEST_ASSERT_EQUALS(uint32_t, b.id, rem->id_autor);
  TEST_ASSERT(strcmp("buenas buenas", rem->mensaje) == 0);
  TEST_ASSERT(rem == t5);

  // bloqueados: a debe estar último
  TEST_ASSERT(user.cantBloqueados = user_copy.cantBloqueados + 1);
  TEST_ASSERT(user.bloqueados[user.cantBloqueados - 1] == &a);

  // No se debe haber modificado nada más
  TEST_ASSERT(usuarios_iguales(&user, &user_copy, false, true, true, false));
  TEST_ASSERT(usuarios_iguales(&a, &a_copy, true, true, true, true));
  TEST_ASSERT(usuarios_iguales(&b, &b_copy, true, true, true, true));

  TEST_ASSERT(tuits_iguales(t2, t2_copy));
  TEST_ASSERT(tuits_iguales(t4, t4_copy));
  TEST_ASSERT(tuits_iguales(t5, t5_copy));

  liberar_tuit(t2);
  liberar_tuit(t4);
  liberar_tuit(t5);
  liberar_tuit(t2_copy);
  liberar_tuit(t4_copy);
  liberar_tuit(t5_copy);
  liberar_usuario(&user);
  liberar_usuario(&b);
  liberar_usuario(&a);
  liberar_usuario(&user_copy);
  liberar_usuario(&b_copy);
  liberar_usuario(&a_copy);
}

TEST(test_ej2_bloquear_bloqueos_multiples) {
  usuario_t user = crear_usuario(10);
  usuario_t a = crear_usuario(100);
  usuario_t b = crear_usuario(200);

  // Feed de user: a -> b -> a
  tuit_t *t1 = crear_tuit("hola", 0, 0, a.id);
  tuit_t *t2 = crear_tuit("chau", 0, 0, b.id);
  tuit_t *t3 = crear_tuit("otra", 0, 0, a.id);
  tuit_t *t4 = crear_tuit("buenas", 0, 0, user.id);
  tuit_t *t5 = crear_tuit("buenas buenas", 0, 0, b.id);
  tuit_t *t6 = crear_tuit("ay ay ay ay", 0, 0, user.id);
  tuit_t *t7 = crear_tuit("canta y no llores", 0, 0, a.id);
  

  user.bloqueados = (usuario_t **)malloc(2 * sizeof(usuario_t*));

  agregar_publicacion(&user, crear_publicacion(t1));
  agregar_publicacion(&user, crear_publicacion(t2));
  agregar_publicacion(&user, crear_publicacion(t3));
  agregar_publicacion(&a, crear_publicacion(t4));
  agregar_publicacion(&a, crear_publicacion(t5));
  agregar_publicacion(&b, crear_publicacion(t6));
  agregar_publicacion(&b, crear_publicacion(t7));

  usuario_t user_copy = clonar_usuario(&user);
  usuario_t a_copy = clonar_usuario(&a);
  usuario_t b_copy = clonar_usuario(&b);

  TEST_CALL_V(bloquearUsuario, &user, &a);
  TEST_CALL_V(bloquearUsuario, &user, &b);

  // Queda solo el de 'b'
  TEST_ASSERT_EQUALS(int32_t, 0, contar_publicaciones(user.feed));
  TEST_ASSERT_EQUALS(int32_t, 1, contar_publicaciones(a.feed));
  TEST_ASSERT_EQUALS(int32_t, 1, contar_publicaciones(b.feed));

  tuit_t* rem = obtener_tuit_en_posicion(a.feed, 0);
  TEST_ASSERT(rem != NULL);
  TEST_ASSERT_EQUALS(uint32_t, b.id, rem->id_autor);
  TEST_ASSERT(strcmp("buenas buenas", rem->mensaje) == 0);
  TEST_ASSERT(rem == t5);

  rem = obtener_tuit_en_posicion(b.feed, 0);
  TEST_ASSERT(rem != NULL);
  TEST_ASSERT_EQUALS(uint32_t, a.id, rem->id_autor);
  TEST_ASSERT(strcmp("canta y no llores", rem->mensaje) == 0);
  TEST_ASSERT(rem == t7);

  // bloqueados: a debe estar último
  TEST_ASSERT(user.cantBloqueados = user_copy.cantBloqueados + 2);
  TEST_ASSERT(user.bloqueados[user.cantBloqueados - 1] == &b);
  TEST_ASSERT(user.bloqueados[user.cantBloqueados - 2] == &a);

  // No se debe haber modificado nada más
  TEST_ASSERT(usuarios_iguales(&user, &user_copy, false, true, true, false));
  TEST_ASSERT(usuarios_iguales(&a, &a_copy, false, true, true, true));
  TEST_ASSERT(usuarios_iguales(&b, &b_copy, false, true, true, true));

  liberar_tuit(t1);
  liberar_tuit(t2);
  liberar_tuit(t3);
  liberar_tuit(t4);
  liberar_tuit(t5);
  liberar_tuit(t6);
  liberar_tuit(t7);
  
  liberar_usuario(&user);
  liberar_usuario(&b);
  liberar_usuario(&a);
  liberar_usuario(&user_copy);
  liberar_usuario(&b_copy);
  liberar_usuario(&a_copy);
}

int main(int argc, char *argv[]) {
  printf("Corriendo los tests del ejercicio 2...\n");

  test_ej2_bloquear_basico();
  test_ej2_bloquear_sin_coincidencias();
  test_ej2_bloquear_todas_coincidencias();
  test_ej2_solo_feed_bloqueado();
  test_ej2_bloquear_solo_feed_bloqueador();
  test_ej2_bloquear_bloqueos_multiples();

  tests_end("Ejercicio 2");
}
