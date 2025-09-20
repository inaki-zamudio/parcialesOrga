#include "ejercicio.h"
#include "../test_utils/test-utils.h"

#include <stdio.h>


// poner esto en 0 permite skipear algunos checks del abi enforcer
#define ABI_CHECK_CALLS 1

#if (WITH_ABI_ENFORCER && ABI_CHECK_CALLS)
// wrap con checks de abi
#define run_hay_accion_que_toque(...)  TEST_CALL_I(hay_accion_que_toque, __VA_ARGS__)
#define run_invocar_acciones(...)      TEST_CALL_V(invocar_acciones,     __VA_ARGS__)
#define run_contar_cartas(...)         TEST_CALL_V(contar_cartas,        __VA_ARGS__)
#else
// llamar directamente
#define run_hay_accion_que_toque  hay_accion_que_toque
#define run_invocar_acciones      invocar_acciones
#define run_contar_cartas         contar_cartas
#endif

void accion_dummy(tablero_t* tablero, carta_t* carta) {}

#define MAX_ACCIONES 256
static int call_count = 0;
static struct {
	tablero_t* tablero;
	carta_t* carta;
	accion_fn_t* fn;
} call_log[MAX_ACCIONES];
static carta_t* last_hurt;

void accion_log(tablero_t* tablero, carta_t* carta) {
	if (MAX_ACCIONES <= call_count) {
		printf("Se invocaron demasiadas acciones!");
	} else {
		call_log[call_count].tablero = tablero;
		call_log[call_count].carta = carta;
		call_log[call_count].fn = accion_log;
	}
	call_count++;
}

void accion_log_and_disable(tablero_t* tablero, carta_t* carta) {
	if (MAX_ACCIONES <= call_count) {
		printf("Se invocaron demasiadas acciones!");
	} else {
		call_log[call_count].tablero = tablero;
		call_log[call_count].carta = carta;
		call_log[call_count].fn = accion_log_and_disable;
	}
	call_count++;
	carta->en_juego = false;
}

void accion_log_and_get_hurt(tablero_t* tablero, carta_t* carta) {
	if (MAX_ACCIONES <= call_count) {
		printf("Se invocaron demasiadas acciones!");
	} else {
		call_log[call_count].tablero = tablero;
		call_log[call_count].carta = carta;
		call_log[call_count].fn = accion_log_and_get_hurt;
	}
	call_count++;
	carta->vida--;
	last_hurt = carta;
}

void accion_log_and_heal(tablero_t* tablero, carta_t* carta) {
	if (MAX_ACCIONES <= call_count) {
		printf("Se invocaron demasiadas acciones!");
	} else {
		call_log[call_count].tablero = tablero;
		call_log[call_count].carta = carta;
		call_log[call_count].fn = accion_log_and_heal;
	}
	call_count++;
	last_hurt->vida += 2;
	last_hurt->en_juego = true;
}

accion_t* crear_cadena_de_acciones(uint16_t cantidad_de_acciones, accion_fn_t** array_de_acciones_fn, carta_t** array_de_cartas) {
	accion_t* primer_accion = (accion_t*) malloc(sizeof(accion_t));

	accion_t* accion_actual = primer_accion;
	accion_actual->invocar = array_de_acciones_fn[0];
	accion_actual->destino = array_de_cartas[0];
	accion_actual->siguiente = NULL;

	accion_t* accion_siguiente = NULL;
	for(int i = 1; i < cantidad_de_acciones; i++){
		accion_siguiente = (accion_t*) malloc(sizeof(accion_t));

		accion_siguiente->invocar = array_de_acciones_fn[i];
		accion_siguiente->destino = array_de_cartas[i];
		accion_siguiente->siguiente = NULL;

		accion_actual->siguiente = accion_siguiente;
		accion_actual = accion_siguiente;
	}

	return primer_accion;

}

void liberar_cadena_de_acciones(accion_t* cadena) {
	accion_t* accion_actual = cadena;
	accion_t* accion_siguiente = NULL;
	while(accion_actual != NULL){
		accion_siguiente = accion_actual->siguiente;
		free(accion_actual);
		accion_actual = accion_siguiente;
	}
}

carta_t* crear_carta(bool en_juego, char* nombre, uint16_t vida, uint8_t jugador){
	carta_t* carta = (carta_t*) malloc(sizeof(carta_t));

	carta->en_juego = en_juego;
	memset(carta->nombre, 0, 12);
	strncpy(carta->nombre, nombre, 11);
	carta->vida = vida;
	carta->jugador = jugador;

	return carta;
}




TEST(ej1_lista_vacía, EJERCICIO_1_HECHO) {
	ASSERT_EQ(bool, false, run_hay_accion_que_toque(NULL, "prueba"));
}

TEST(ej1_está_en_lista_de_un_elem, EJERCICIO_1_HECHO) {
	carta_t* carta =  crear_carta(true, "Prueba", 100, JUGADOR_ROJO);
	accion_t* accion = crear_cadena_de_acciones(
		1,
		(accion_fn_t*[]) { accion_dummy },
		(carta_t*[]) { carta }
	);

	ASSERT_EQ(bool, true, run_hay_accion_que_toque(accion, "Prueba"));

	liberar_cadena_de_acciones(accion);
	free(carta);
}

TEST(ej1_no_está_en_lista_de_un_elem, EJERCICIO_1_HECHO) {
	carta_t* carta =  crear_carta(true, "otra cosa", 100, JUGADOR_ROJO);
	accion_t* accion = crear_cadena_de_acciones(
		1,
		(accion_fn_t*[]) { accion_dummy },
		(carta_t*[]) { carta }
	);

	ASSERT_EQ(bool, false, run_hay_accion_que_toque(accion, "prueba"));

	liberar_cadena_de_acciones(accion);
	free(carta);
}

TEST(ej1_está_en_lista_larga, EJERCICIO_1_HECHO) {
	carta_t* carta_que_no =  crear_carta(true, "esteno", 100, JUGADOR_ROJO);
	carta_t* carta_que_si =  crear_carta(true, "estesi", 100, JUGADOR_ROJO);
	accion_t* acciones = crear_cadena_de_acciones(
		6,
		(accion_fn_t*[]) { accion_dummy, accion_dummy, accion_dummy, accion_dummy, accion_dummy, accion_dummy },
		(carta_t*[]) { carta_que_no, carta_que_no, carta_que_no, carta_que_no, carta_que_si, carta_que_no }
	);

	ASSERT_EQ(bool, true, run_hay_accion_que_toque(acciones, "estesi"));

	liberar_cadena_de_acciones(acciones);
	free(carta_que_no);
	free(carta_que_si);
}

TEST(ej1_no_está_en_lista_larga, EJERCICIO_1_HECHO) {
	carta_t* carta_que_no =  crear_carta(true, "esteno", 100, JUGADOR_ROJO);
	accion_t* acciones = crear_cadena_de_acciones(
		6,
		(accion_fn_t*[]) { accion_dummy, accion_dummy, accion_dummy, accion_dummy, accion_dummy, accion_dummy },
		(carta_t*[]) { carta_que_no, carta_que_no, carta_que_no, carta_que_no, carta_que_no, carta_que_no }
	);

	ASSERT_EQ(bool, false, run_hay_accion_que_toque(acciones, "encontrame"));

	liberar_cadena_de_acciones(acciones);
	free(carta_que_no);
}

TEST(ej2_lista_vacía, EJERCICIO_2_HECHO) {
	tablero_t tablero = {};

	call_count = 0;
	run_invocar_acciones(NULL, &tablero);

	ASSERT_EQ(int, 0, call_count);
}

TEST(ej2_lista_chica_todo_en_juego, EJERCICIO_2_HECHO) {
	tablero_t tablero = {};
	carta_t* carta0 =  crear_carta(true, "carta0", 100, JUGADOR_ROJO);
	carta_t* carta1 =  crear_carta(true, "carta1", 100, JUGADOR_AZUL);
	carta_t* carta2 =  crear_carta(true, "carta2", 100, JUGADOR_ROJO);
	accion_t* acciones = crear_cadena_de_acciones(3,
		(accion_fn_t*[]) { accion_log, accion_log, accion_log },
		(carta_t*[]) { carta0, carta1, carta2 }
	);

	call_count = 0;
	run_invocar_acciones(acciones, &tablero);

	ASSERT_EQ(int, 3, call_count);
	ASSERT_EQ(void*, acciones->destino,                       call_log[0].carta);
	ASSERT_EQ(void*, acciones->siguiente->destino,            call_log[1].carta);
	ASSERT_EQ(void*, acciones->siguiente->siguiente->destino, call_log[2].carta);
	ASSERT_EQ(void*, &tablero, call_log[0].tablero);
	ASSERT_EQ(void*, &tablero, call_log[1].tablero);
	ASSERT_EQ(void*, &tablero, call_log[2].tablero);
	ASSERT_EQ(void*, accion_log, call_log[0].fn);
	ASSERT_EQ(void*, accion_log, call_log[1].fn);
	ASSERT_EQ(void*, accion_log, call_log[2].fn);

	liberar_cadena_de_acciones(acciones);
	free(carta0);
	free(carta1);
	free(carta2);
}

TEST(ej2_lista_chica_todo_fuera_de_juego, EJERCICIO_2_HECHO) {
	tablero_t tablero = {};
	carta_t* carta0 =  crear_carta(false, "carta0", 100, JUGADOR_ROJO);
	carta_t* carta1 =  crear_carta(false, "carta1", 100, JUGADOR_AZUL);
	carta_t* carta2 =  crear_carta(false, "carta2", 100, JUGADOR_ROJO);
	accion_t* acciones = crear_cadena_de_acciones(3,
		(accion_fn_t*[]) { accion_log, accion_log, accion_log },
		(carta_t*[]) { carta0, carta1, carta2 }
	);

	call_count = 0;
	run_invocar_acciones(&acciones[0], &tablero);

	ASSERT_EQ(int, 0, call_count);

	liberar_cadena_de_acciones(acciones);
	free(carta0);
	free(carta1);
	free(carta2);
}

TEST(ej2_carta_se_deshabilita, EJERCICIO_2_HECHO) {
	tablero_t tablero = {};
	carta_t* carta0 =  crear_carta(true, "carta0", 100, JUGADOR_ROJO);
	carta_t* carta1 =  crear_carta(true, "carta1", 50, JUGADOR_ROJO);
	accion_t* acciones = crear_cadena_de_acciones(
		7,
		(accion_fn_t*[]) { accion_log, accion_log_and_disable, accion_log, accion_log, accion_log, accion_log_and_disable, accion_log },
		(carta_t*[]) { carta0, carta0, carta0, carta1, carta0, carta1, carta1 }
	);

	call_count = 0;
	run_invocar_acciones(acciones, &tablero);

	ASSERT_EQ(int, 4, call_count);
	ASSERT_EQ(void*, carta0, call_log[0].carta);
	ASSERT_EQ(void*, carta0, call_log[1].carta);
	ASSERT_EQ(void*, carta1, call_log[2].carta);
	ASSERT_EQ(void*, carta1, call_log[3].carta);
	ASSERT_EQ(void*, &tablero, call_log[0].tablero);
	ASSERT_EQ(void*, &tablero, call_log[1].tablero);
	ASSERT_EQ(void*, &tablero, call_log[2].tablero);
	ASSERT_EQ(void*, &tablero, call_log[3].tablero);
	ASSERT_EQ(void*, accion_log,             call_log[0].fn);
	ASSERT_EQ(void*, accion_log_and_disable, call_log[1].fn);
	ASSERT_EQ(void*, accion_log,             call_log[2].fn);
	ASSERT_EQ(void*, accion_log_and_disable, call_log[3].fn);

	liberar_cadena_de_acciones(acciones);
	free(carta0);
	free(carta1);
}

TEST(ej2_carta_sin_vida_se_invoca, EJERCICIO_2_HECHO) {
	tablero_t tablero = {};
	carta_t* carta = crear_carta(true, "Moribunda", 0, JUGADOR_ROJO);
	accion_t* acciones = crear_cadena_de_acciones(
		2,
		(accion_fn_t*[]) { accion_log, accion_log },
		(carta_t*[]) { carta, carta }
	);

	call_count = 0;
	run_invocar_acciones(acciones, &tablero);

	ASSERT_EQ(int, 1, call_count);
	ASSERT_EQ(void*, carta,      call_log[0].carta);
	ASSERT_EQ(void*, &tablero,   call_log[0].tablero);
	ASSERT_EQ(void*, accion_log, call_log[0].fn);
	ASSERT_EQ(bool, false, carta->en_juego);

	liberar_cadena_de_acciones(acciones);
	free(carta);
}

TEST(ej2_carta_revivida_se_invoca, EJERCICIO_2_HECHO) {
	tablero_t tablero = {};
	carta_t* emo = crear_carta(true, "Emo", 2, JUGADOR_ROJO);
	carta_t* healer = crear_carta(true, "Lucio", 5, JUGADOR_ROJO);
	accion_t* acciones = crear_cadena_de_acciones(
		7,
		(accion_fn_t*[]) { accion_log_and_get_hurt, accion_log_and_get_hurt, accion_log_and_get_hurt, accion_log_and_heal, accion_log_and_get_hurt, accion_log_and_get_hurt, accion_log_and_get_hurt },
		(carta_t*[]) { emo, emo, emo, healer, emo, emo, emo }
	);

	call_count = 0;
	run_invocar_acciones(acciones, &tablero);

	ASSERT_EQ(int, 5, call_count);
	ASSERT_EQ(void*, emo,    call_log[0].carta);
	ASSERT_EQ(void*, emo,    call_log[1].carta);
	ASSERT_EQ(void*, healer, call_log[2].carta);
	ASSERT_EQ(void*, emo,    call_log[3].carta);
	ASSERT_EQ(void*, emo,    call_log[4].carta);
	ASSERT_EQ(void*, &tablero, call_log[0].tablero);
	ASSERT_EQ(void*, &tablero, call_log[1].tablero);
	ASSERT_EQ(void*, &tablero, call_log[2].tablero);
	ASSERT_EQ(void*, &tablero, call_log[3].tablero);
	ASSERT_EQ(void*, &tablero, call_log[4].tablero);
	ASSERT_EQ(void*, accion_log_and_get_hurt, call_log[0].fn);
	ASSERT_EQ(void*, accion_log_and_get_hurt, call_log[1].fn);
	ASSERT_EQ(void*, accion_log_and_heal,     call_log[2].fn);
	ASSERT_EQ(void*, accion_log_and_get_hurt, call_log[3].fn);
	ASSERT_EQ(void*, accion_log_and_get_hurt, call_log[4].fn);
	ASSERT_EQ(bool,     false, emo->en_juego);
	ASSERT_EQ(uint32_t, 0,     emo->vida);
	ASSERT_EQ(bool,     true, healer->en_juego);
	ASSERT_EQ(uint32_t, 5,    healer->vida);

	liberar_cadena_de_acciones(acciones);
	free(emo);
	free(healer);
}

TEST(ej3_tablero_vacío, EJERCICIO_3_HECHO) {
	carta_t rojo[] = {};
	carta_t azul[] = {};
	tablero_t tablero = {};
	tablero.mano_jugador_rojo = rojo;
	tablero.mano_jugador_azul = azul;
	uint32_t cant_rojo = 0;
	uint32_t cant_azul = 0;

	run_contar_cartas(&tablero, &cant_rojo, &cant_azul);

	ASSERT_EQ(uint32_t, 0, cant_rojo);
	ASSERT_EQ(uint32_t, 0, cant_azul);
}

TEST(ej3_seis_y_tres, EJERCICIO_3_HECHO) {
	carta_t* rojo = malloc(sizeof(carta_t[6]));
	rojo[0] = (carta_t) {
		.en_juego = true,
		.nombre = "Carta0",
		.vida = 10,
		.jugador = JUGADOR_ROJO
	};
	rojo[1] = (carta_t) {
		.en_juego = true,
		.nombre = "Carta1",
		.vida = 10,
		.jugador = JUGADOR_ROJO
	};
	rojo[2] = (carta_t) {
		.en_juego = true,
		.nombre = "Carta2",
		.vida = 10,
		.jugador = JUGADOR_ROJO
	};
	rojo[3] = (carta_t) {
		.en_juego = true,
		.nombre = "Carta3",
		.vida = 10,
		.jugador = JUGADOR_ROJO
	};
	rojo[4] = (carta_t) {
		.en_juego = true,
		.nombre = "Carta4",
		.vida = 10,
		.jugador = JUGADOR_ROJO
	};
	rojo[5] = (carta_t) {
		.en_juego = true,
		.nombre = "Carta5",
		.vida = 10,
		.jugador = JUGADOR_ROJO
	};
	carta_t* azul = malloc(sizeof(carta_t[3]));
	azul[0] = (carta_t) {
		.en_juego = true,
		.nombre = "Carta6",
		.vida = 10,
		.jugador = JUGADOR_AZUL
	};
	azul[1] = (carta_t) {
		.en_juego = true,
		.nombre = "Carta7",
		.vida = 10,
		.jugador = JUGADOR_AZUL
	};
	azul[2] = (carta_t) {
		.en_juego = true,
		.nombre = "Carta8",
		.vida = 10,
		.jugador = JUGADOR_AZUL
	};
	tablero_t* tablero = malloc(sizeof(tablero_t));
	*tablero = (tablero_t) {};
	tablero->mano_jugador_rojo = rojo;
	tablero->mano_jugador_azul = azul;
	tablero->campo[3][3] = &rojo[0];
	tablero->campo[0][2] = &rojo[1];
	tablero->campo[2][1] = &rojo[2];
	tablero->campo[4][9] = &rojo[3];
	tablero->campo[4][0] = &rojo[4];
	tablero->campo[2][6] = &rojo[5];
	tablero->campo[0][0] = &azul[0];
	tablero->campo[1][1] = &azul[1];
	tablero->campo[3][7] = &azul[2];
	// Esto es completamente innecesario, lo lógico sería pasar una
	// variable de la pila. El tema es que si hacemos eso valgrind no tiene
	// forma fácil de darse cuenta si un estudiante escribió afuera del
	// buffer.
	uint32_t* cant_rojo = malloc(sizeof(uint32_t));
	uint32_t* cant_azul = malloc(sizeof(uint32_t));
	// Las inicializamos en este tests porque somos buena gente
	*cant_rojo = *cant_azul = 0;

	run_contar_cartas(tablero, cant_rojo, cant_azul);

	ASSERT_EQ(uint32_t, 6, *cant_rojo);
	ASSERT_EQ(uint32_t, 3, *cant_azul);

	free(rojo);
	free(azul);
	free(tablero);
	free(cant_rojo);
	free(cant_azul);
}

TEST(ej3_seis_y_tres_contadores_con_basura, EJERCICIO_3_HECHO) {
	carta_t* rojo = malloc(sizeof(carta_t[6]));
	rojo[0] = (carta_t) {
		.en_juego = true,
		.nombre = "Carta0",
		.vida = 10,
		.jugador = JUGADOR_ROJO
	};
	rojo[1] = (carta_t) {
		.en_juego = true,
		.nombre = "Carta1",
		.vida = 10,
		.jugador = JUGADOR_ROJO
	};
	rojo[2] = (carta_t) {
		.en_juego = true,
		.nombre = "Carta2",
		.vida = 10,
		.jugador = JUGADOR_ROJO
	};
	rojo[3] = (carta_t) {
		.en_juego = true,
		.nombre = "Carta3",
		.vida = 10,
		.jugador = JUGADOR_ROJO
	};
	rojo[4] = (carta_t) {
		.en_juego = true,
		.nombre = "Carta4",
		.vida = 10,
		.jugador = JUGADOR_ROJO
	};
	rojo[5] = (carta_t) {
		.en_juego = true,
		.nombre = "Carta5",
		.vida = 10,
		.jugador = JUGADOR_ROJO
	};
	carta_t* azul = malloc(sizeof(carta_t[3]));
	azul[0] = (carta_t) {
		.en_juego = true,
		.nombre = "Carta6",
		.vida = 10,
		.jugador = JUGADOR_AZUL
	};
	azul[1] = (carta_t) {
		.en_juego = true,
		.nombre = "Carta7",
		.vida = 10,
		.jugador = JUGADOR_AZUL
	};
	azul[2] = (carta_t) {
		.en_juego = true,
		.nombre = "Carta8",
		.vida = 10,
		.jugador = JUGADOR_AZUL
	};
	tablero_t* tablero = malloc(sizeof(tablero_t));
	*tablero = (tablero_t) {};
	tablero->mano_jugador_rojo = rojo;
	tablero->mano_jugador_azul = azul;
	tablero->campo[3][3] = &rojo[0];
	tablero->campo[0][2] = &rojo[1];
	tablero->campo[2][1] = &rojo[2];
	tablero->campo[4][9] = &rojo[3];
	tablero->campo[4][0] = &rojo[4];
	tablero->campo[2][6] = &rojo[5];
	tablero->campo[0][0] = &azul[0];
	tablero->campo[1][1] = &azul[1];
	tablero->campo[3][7] = &azul[2];
	// Esto es completamente innecesario, lo lógico sería pasar una
	// variable de la pila. El tema es que si hacemos eso valgrind no tiene
	// forma fácil de darse cuenta si un estudiante escribió afuera del
	// buffer.
	uint32_t* cant_rojo = malloc(sizeof(uint32_t));
	uint32_t* cant_azul = malloc(sizeof(uint32_t));
	// No las inicializamos en este test porque somos mala gente

	run_contar_cartas(tablero, cant_rojo, cant_azul);

	ASSERT_EQ(uint32_t, 6, *cant_rojo);
	ASSERT_EQ(uint32_t, 3, *cant_azul);

	free(rojo);
	free(azul);
	free(tablero);
	free(cant_rojo);
	free(cant_azul);
}

TEST(ej3_cartas_fuera_de_campo, EJERCICIO_3_HECHO) {
	carta_t* rojo = malloc(sizeof(carta_t[7]));
	rojo[0] = (carta_t) {
		.en_juego = true,
		.nombre = "Carta0",
		.vida = 10,
		.jugador = JUGADOR_ROJO
	};
	rojo[1] = (carta_t) {
		.en_juego = true,
		.nombre = "Carta1",
		.vida = 10,
		.jugador = JUGADOR_ROJO
	};
	rojo[2] = (carta_t) {
		.en_juego = true,
		.nombre = "Carta2",
		.vida = 10,
		.jugador = JUGADOR_ROJO
	};
	rojo[3] = (carta_t) {
		.en_juego = true,
		.nombre = "Carta3",
		.vida = 10,
		.jugador = JUGADOR_ROJO
	};
	rojo[4] = (carta_t) {
		.en_juego = true,
		.nombre = "Carta4",
		.vida = 10,
		.jugador = JUGADOR_ROJO
	};
	rojo[5] = (carta_t) {
		.en_juego = true,
		.nombre = "Carta5",
		.vida = 10,
		.jugador = JUGADOR_ROJO
	};
	rojo[6] = (carta_t) {
		.en_juego = true,
		.nombre = "Carta6",
		.vida = 10,
		.jugador = JUGADOR_ROJO
	};
	carta_t* azul = malloc(sizeof(carta_t[5]));
	azul[0] = (carta_t) {
		.en_juego = true,
		.nombre = "Carta7",
		.vida = 10,
		.jugador = JUGADOR_AZUL
	};
	azul[1] = (carta_t) {
		.en_juego = true,
		.nombre = "Carta8",
		.vida = 10,
		.jugador = JUGADOR_AZUL
	};
	azul[2] = (carta_t) {
		.en_juego = true,
		.nombre = "Carta9",
		.vida = 10,
		.jugador = JUGADOR_AZUL
	};
	azul[3] = (carta_t) {
		.en_juego = true,
		.nombre = "Carta10",
		.vida = 10,
		.jugador = JUGADOR_AZUL
	};
	azul[4] = (carta_t) {
		.en_juego = true,
		.nombre = "Carta11",
		.vida = 10,
		.jugador = JUGADOR_AZUL
	};
	tablero_t* tablero = malloc(sizeof(tablero_t));
	*tablero = (tablero_t) {};
	tablero->mano_jugador_rojo = rojo;
	tablero->mano_jugador_azul = azul;
	tablero->campo[3][3] = &rojo[0];
	tablero->campo[0][2] = &rojo[1];
	tablero->campo[2][1] = &rojo[4];
	tablero->campo[2][6] = &rojo[6];
	tablero->campo[0][0] = &azul[0];
	tablero->campo[1][1] = &azul[1];
	tablero->campo[3][7] = &azul[3];
	// Esto es completamente innecesario, lo lógico sería pasar una
	// variable de la pila. El tema es que si hacemos eso valgrind no tiene
	// forma fácil de darse cuenta si un estudiante escribió afuera del
	// buffer.
	uint32_t* cant_rojo = malloc(sizeof(uint32_t));
	uint32_t* cant_azul = malloc(sizeof(uint32_t));
	// Las inicializamos "mal" en test porque somos muy mala gente
	*cant_rojo = *cant_azul = -1;

	run_contar_cartas(tablero, cant_rojo, cant_azul);

	ASSERT_EQ(uint32_t, 4, *cant_rojo);
	ASSERT_EQ(uint32_t, 3, *cant_azul);

	free(rojo);
	free(azul);
	free(tablero);
	free(cant_rojo);
	free(cant_azul);
}

TEST(ej3_hay_jugador_verde, EJERCICIO_3_HECHO) {
	carta_t* rojo = malloc(sizeof(carta_t[2]));
	rojo[0] = (carta_t) {
		.en_juego = true,
		.nombre = "Carta0",
		.vida = 10,
		.jugador = JUGADOR_ROJO
	};
	rojo[1] = (carta_t) {
		.en_juego = true,
		.nombre = "Carta1",
		.vida = 10,
		.jugador = JUGADOR_ROJO
	};
	carta_t* azul = malloc(sizeof(carta_t[1]));
	azul[0] = (carta_t) {
		.en_juego = true,
		.nombre = "Carta2",
		.vida = 10,
		.jugador = JUGADOR_AZUL
	};
	carta_t* verde0 = crear_carta(true, "NOCUENTA",  10, 99 /* Un jugador que no es ni el ROJO ni el AZUL */);
	carta_t* verde1 = crear_carta(true, "NOCUENTA2", 10, 99 /* Un jugador que no es ni el ROJO ni el AZUL */);
	tablero_t* tablero = malloc(sizeof(tablero_t));
	*tablero = (tablero_t) {};
	tablero->mano_jugador_rojo = rojo;
	tablero->mano_jugador_azul = azul;
	tablero->campo[3][3] = &rojo[0];
	tablero->campo[0][2] = &rojo[1];
	tablero->campo[0][0] = &azul[0];
	tablero->campo[2][6] = verde0;
	tablero->campo[1][9] = verde1;
	// Esto es completamente innecesario, lo lógico sería pasar una
	// variable de la pila. El tema es que si hacemos eso valgrind no tiene
	// forma fácil de darse cuenta si un estudiante escribió afuera del
	// buffer.
	uint32_t* cant_rojo = malloc(sizeof(uint32_t));
	uint32_t* cant_azul = malloc(sizeof(uint32_t));
	// No las inicializamos en este test porque somos mala gente

	run_contar_cartas(tablero, cant_rojo, cant_azul);

	ASSERT_EQ(uint32_t, 2, *cant_rojo);
	ASSERT_EQ(uint32_t, 1, *cant_azul);

	free(rojo);
	free(azul);
	free(tablero);
	free(verde0);
	free(verde1);
	free(cant_rojo);
	free(cant_azul);
}

int main(void) {
#ifndef EXCLUDE_EJ1
	printf("== Ejercicio 1 ==\n");
	ej1_lista_vacía();
	ej1_está_en_lista_de_un_elem();
	ej1_no_está_en_lista_de_un_elem();
	ej1_está_en_lista_larga();
	ej1_no_está_en_lista_larga();
	printf("\n");
#endif

#ifndef EXCLUDE_EJ2
	printf("== Ejercicio 2 ==\n");
	ej2_lista_vacía();
	ej2_lista_chica_todo_en_juego();
	ej2_lista_chica_todo_fuera_de_juego();
	ej2_carta_se_deshabilita();
	ej2_carta_sin_vida_se_invoca();
	ej2_carta_revivida_se_invoca();
	printf("\n");
#endif

#ifndef EXCLUDE_EJ3
	printf("== Ejercicio 3 ==\n");
	ej3_tablero_vacío();
	ej3_seis_y_tres();
	ej3_seis_y_tres_contadores_con_basura();
	ej3_cartas_fuera_de_campo();
	ej3_hay_jugador_verde();
#endif

	tests_end("el parcial");
	return 0;
}
