#pragma once
#include <assert.h> //provee la macro assert que evalúa una condición, y si no se cumple provee información diagnóstica y aborta la ejecución
#include <ctype.h> //contiene funciones relacionadas a caracteres, isdigit, islower, tolower...
#include <math.h> //define funciones matemáticas como cos, sin, abs, sqrt, log...
#include <stdbool.h> //contiene las definiciones de datos booleanos, true (1), false (0)
#include <stdint.h> //contiene la definición de tipos enteros ligados a tamaños int8_t, int16_t, uint8_t,...
#include <stdio.h> //encabezado de funciones de entrada y salida fopen, fclose, fgetc, printf, fprintf ...
#include <stdlib.h> //biblioteca estándar, atoi, atof, rand, srand, abort, exit, system, NULL, malloc, calloc, realloc...
#include <string.h> //contiene las funciones relacionadas a strings, memcmp, strcat, memset, memmove, strlen,strstr...
#include <unistd.h> //define constantes y tipos standard, NULL, R_OK, F_OK, STDIN_FILENO, STDOUT_FILENO, STDERR_FILENO...

//*************************************
// Declaración de estructuras de la red social Twiner
//*************************************

// Forward declarations
typedef struct usuario_s usuario_t;
typedef struct feed_s feed_t;
typedef struct publicacion_s publicacion_t;
typedef struct tuit_s tuit_t;

// Estructura tuit
typedef struct tuit_s {
  char mensaje[140];       // asmdef_offset:TUIT_MENSAJE_OFFSET
  uint16_t favoritos;      // asmdef_offset:TUIT_FAVORITOS_OFFSET
  uint16_t retuits;        // asmdef_offset:TUIT_RETUITS_OFFSET
  uint32_t id_autor;       // asmdef_offset:TUIT_ID_AUTOR_OFFSET
} tuit_t;                  // asmdef_offset:TUIT_SIZE

// Estructura publicacion
typedef struct publicacion_s {
  publicacion_t *next;     // asmdef_offset:PUBLICACION_NEXT_OFFSET
  tuit_t *value;           // asmdef_offset:PUBLICACION_VALUE_OFFSET
} publicacion_t;           // asmdef_offset:PUBLICACION_SIZE

// Estructura feed
typedef struct feed_s {
  publicacion_t *first;    // asmdef_offset:FEED_FIRST_OFFSET
} feed_t;                  // asmdef_offset:FEED_SIZE

// Estructura usuario
typedef struct usuario_s {
  feed_t *feed;            // asmdef_offset:USUARIO_FEED_OFFSET
  usuario_t **seguidores;  // asmdef_offset:USUARIO_SEGUIDORES_OFFSET
  uint32_t cantSeguidores; // asmdef_offset:USUARIO_CANT_SEGUIDORES_OFFSET
  usuario_t **seguidos;    // asmdef_offset:USUARIO_SEGUIDOS_OFFSET
  uint32_t cantSeguidos;   // asmdef_offset:USUARIO_CANT_SEGUIDOS_OFFSET
  usuario_t **bloqueados;  // asmdef_offset:USUARIO_BLOQUEADOS_OFFSET
  uint32_t cantBloqueados; // asmdef_offset:USUARIO_CANT_BLOQUEADOS_OFFSET
  uint32_t id;             // asmdef_offset:USUARIO_ID_OFFSET
} usuario_t;               // asmdef_offset:USUARIO_SIZE

//*************************************
// Declaración de funciones de los ejercicios
//*************************************

tuit_t *publicar(char *mensaje, usuario_t *usuario);

void bloquearUsuario(usuario_t *usuario, usuario_t *usuarioABloquear);

tuit_t **trendingTopic(usuario_t *usuario,
                       uint8_t (*esTuitSobresaliente)(tuit_t *));
