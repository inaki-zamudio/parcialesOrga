#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ej1.h"
#include "../../test_utils/test-utils.h"

/**
 * Cuenta cuántos tests corrieron exitosamente.
 */
uint64_t successful_tests = 0;
/**
 * Cuenta cuántos tests test fallaron.
 */
uint64_t failed_tests = 0;

/**
 * El mensaje [DONE] escrito en verde.
 */
#define DONE "[\033[32;1mDONE\033[0m] "

/**
 * El mensaje [FAIL] escrito en rojo.
 */
#define FAIL "[\033[31;1mFAIL\033[0m] "

/**
 * El mensaje [INFO] escrito en amarillo.
 */
#define INFO "[\033[33;1mINFO\033[0m] "

/**
 * El mensaje [SKIP] escrito en magenta.
 */
#define SKIP "[\033[95;1mSKIP\033[0m] "

uint32_t hash(attackunit_t* p) {
    uint32_t res = p->combustible;
    if (strcmp(p->clase, "anti-air") == 0) {
        res += 1000;
    } else if (strcmp(p->clase, "artillery") == 0) {
        res += 2000;
    } else if (strcmp(p->clase, "battleship") == 0) {
        res += 3000;
    } else if (strcmp(p->clase, "mech") == 0) {
        res += 4000;
    }
    return res;
}

uint16_t get_combustible(char* clase) {
	// Referencia: https://awbw.fandom.com/wiki/Units
    uint16_t res;
    if (strcmp(clase, "anti-air") == 0) {
        res = 60;
    } else if (strcmp(clase, "artillery") == 0) {
        res = 50;
    } else if (strcmp(clase, "battleship") == 0) {
        res = 99;
    } else if (strcmp(clase, "mech") == 0) {
        res = 70;
    } else {
		// No debería caer acá pero por las dudas
		res = 20;
	}
    return res;
}

void fun_modificar(attackunit_t* unidad) {
	unidad->combustible -= 10;
	if (strcmp(unidad->clase, "mech") == 0)
		unidad->combustible -= 10;
}

#define P(valor_clase, valor_combustible, valor_references) (&(attackunit_t) { \
	.clase = #valor_clase,                                          \
	.combustible = (valor_combustible),                                                 \
	.references = (valor_references)                                  \
})

void print_map(mapa_t mapa) {
    // Print headers
    printf("%-30s %-30s\n", "Hashes", "Punteros (5 últimos dígitos)");
    printf("%-30s %-30s\n", "-----------------------------", "-------------------------------");

    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            // Print hash or 0 if NULL
            if (mapa[i][j] != NULL) {
                printf("%5u ", hash(mapa[i][j]));
            } else {
                printf("%5d ", 0);
            }
        }

        // Align second column for pointers
        printf("   ");

        for (int j = 0; j < 5; j++) {
            // Print last 5 digits of pointer or 0 if NULL
            if (mapa[i][j] != NULL) {
                printf("%5lu ", (unsigned long)mapa[i][j] % 100000);
            } else {
                printf("%5d ", 0);
            }
        }

        // Move to the next row
        printf("\n");
    }
    printf("\n");
}

void print_map_combustible(mapa_t mapa) {
    // Print headers
    printf("%-30s %-30s\n", "Combustible base", "Combustible");
    printf("%-30s %-30s\n", "-----------------------------", "-------------------------------");

    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            // Print hash or 0 if NULL
            if (mapa[i][j] != NULL) {
                printf("%5u ", get_combustible((char*) mapa[i][j]->clase));
            } else {
                printf("%5d ", 0);
            }
        }

        // Align second column for pointers
        printf("   ");

        for (int j = 0; j < 5; j++) {
            // Print last 5 digits of pointer or 0 if NULL
            if (mapa[i][j] != NULL) {
                printf("%5u ", mapa[i][j]->combustible);
            } else {
                printf("%5d ", 0);
            }
        }

        // Move to the next row
        printf("\n");
    }
    printf("\n");
}

void testResults1A(mapa_t esperado, mapa_t resultado, bool cantidad_refs_correcta, char* testName) {
	bool iguales = cantidad_refs_correcta;
	uint32_t i;
	for (i=0; iguales && i<255*255; i++) {
		iguales = iguales && ((uint64_t*) esperado)[i] == ((uint64_t*) resultado)[i];
	}

	if (iguales) {
		successful_tests++;
		printf(DONE "%s: el mapa esperado y el obtenido coinciden.\n",testName);
	} else {
		failed_tests++;		
		if (!cantidad_refs_correcta) {
			printf(FAIL "%s: cantidad de referencias incorrecta.\n", testName);
		} else {
			printf(FAIL "%s: el mapa esperado y el obtenido no coinciden.\n", testName);
		}
		printf("Se esperaba un mapa con las características:\n");
		print_map(esperado);
		printf("Pero se obtuvo el siguiente mapa:\n");
		print_map(resultado);
	}
}

void test_1a_varios_cambios() {
	mapa_t mapa = {0};
	mapa[0][0] = malloc(sizeof(attackunit_t));
	*mapa[0][0] = *P(anti-air, 60, 1);
	mapa[1][1] = malloc(sizeof(attackunit_t));
	*mapa[1][1] = *P(anti-air, 60, 1);
	mapa[4][3] = malloc(sizeof(attackunit_t));
	*mapa[4][3] = *P(artillery, 50, 1);
	mapa[3][4] = malloc(sizeof(attackunit_t));
	*mapa[3][4] = *P(anti-air, 60, 1);
	mapa[4][4] = malloc(sizeof(attackunit_t));
	*mapa[4][4] = *P(battleship, 99, 1);

	mapa_t esperado = {0};
	esperado[0][0] = mapa[1][1];
	esperado[1][1] = mapa[1][1];
	esperado[4][3] = mapa[4][3];
	esperado[3][4] = mapa[1][1];
	esperado[4][4] = mapa[4][4];

	optimizar(mapa, mapa[1][1], hash);
	testResults1A(esperado, mapa, mapa[1][1]->references == 3, "test_1a_varios_cambios");

	free(mapa[1][1]);
	free(mapa[4][3]);
	free(mapa[4][4]);
}

void test_1a_instancia_compartida_nueva() {
	mapa_t mapa = {0};
	mapa[0][0] = malloc(sizeof(attackunit_t));
	*mapa[0][0] = *P(anti-air, 60, 1);
	mapa[1][1] = malloc(sizeof(attackunit_t));
	*mapa[1][1] = *P(anti-air, 60, 1);
	mapa[4][3] = malloc(sizeof(attackunit_t));
	*mapa[4][3] = *P(artillery, 50, 1);
	mapa[3][4] = malloc(sizeof(attackunit_t));
	*mapa[3][4] = *P(anti-air, 60, 1);
	mapa[4][4] = malloc(sizeof(attackunit_t));
	*mapa[4][4] = *P(battleship, 99, 1);

	attackunit_t* compartida = malloc(sizeof(attackunit_t));
	*compartida = *P(anti-air, 60, 0);

	mapa_t esperado = {0};
	esperado[0][0] = compartida;
	esperado[1][1] = compartida;
	esperado[4][3] = mapa[4][3];
	esperado[3][4] = compartida;
	esperado[4][4] = mapa[4][4];

	optimizar(mapa, compartida, hash);
	testResults1A(esperado, mapa, compartida->references == 3, "test_1a_instancia_compartida_nueva");

	free(compartida);
	free(mapa[4][3]);
	free(mapa[4][4]);
}

void test_1a_sin_cambios() {
	mapa_t mapa = {0};
	mapa[0][0] = malloc(sizeof(attackunit_t));
	*mapa[0][0] = *P(anti-air, 60, 1);
	mapa[1][1] = malloc(sizeof(attackunit_t));
	*mapa[1][1] = *P(mech, 70, 1);
	mapa[3][4] = malloc(sizeof(attackunit_t));
	*mapa[3][4] = *P(artillery, 20, 1);
	mapa[4][4] = malloc(sizeof(attackunit_t));
	*mapa[4][4] = *P(battleship, 99, 1);

	mapa_t esperado = {0};
	esperado[0][0] = mapa[0][0];
	esperado[1][1] = mapa[1][1];
	esperado[3][4] = mapa[3][4];
	esperado[4][4] = mapa[4][4];

	optimizar(mapa, mapa[0][0], hash);
	testResults1A(esperado, mapa, mapa[0][0]->references == 1, "test_1a_sin_cambios");

	free(mapa[0][0]);
	free(mapa[1][1]);
	free(mapa[3][4]);
	free(mapa[4][4]);
}

void test_1a_optimizo_inst_optimizada() {
	mapa_t mapa = {0};
	mapa[0][0] = malloc(sizeof(attackunit_t));
	*mapa[0][0] = *P(battleship, 99, 2);
	mapa[1][1] = malloc(sizeof(attackunit_t));
	*mapa[1][1] = *P(mech, 70, 1);
	mapa[3][4] = malloc(sizeof(attackunit_t));
	*mapa[3][4] = *P(battleship, 99, 1);
	mapa[4][4] = mapa[0][0];

	attackunit_t* compartida = malloc(sizeof(attackunit_t));
	*compartida = *P(battleship, 99, 0);

	mapa_t esperado = {0};
	esperado[0][0] = compartida;
	esperado[1][1] = mapa[1][1];
	esperado[3][4] = compartida;
	esperado[4][4] = compartida;

	optimizar(mapa, compartida, hash);
	testResults1A(esperado, mapa, compartida->references == 3, "test_1a_optimizo_inst_optimizada");

	free(mapa[0][0]);
	free(mapa[1][1]);
}

void test_1a_mapa_completo(uint64_t successful_at_start) {
	if (successful_tests - successful_at_start < 4) {
		printf(FAIL "El test sobre el mapa completo se ejecutará cuando pasen los tests anteriores.\n");
		return;
	}
	mapa_t mapa = {0};
	mapa[0][0] = malloc(sizeof(attackunit_t));
	*mapa[0][0] = *P(anti-air, 60, 1);
	mapa[43][89] = malloc(sizeof(attackunit_t));
	*mapa[43][89] = *P(anti-air, 60, 1);
	mapa[100][100] = malloc(sizeof(attackunit_t));
	*mapa[100][100] = *P(anti-air, 60, 1);
	mapa[201][10] = malloc(sizeof(attackunit_t));
	*mapa[201][10] = *P(anti-air, 60, 1);
	mapa[241][241] = malloc(sizeof(attackunit_t));
	*mapa[241][241] = *P(anti-air, 60, 1);
	mapa[4][3] = malloc(sizeof(attackunit_t));
	*mapa[4][3] = *P(artillery, 50, 1);
	mapa[3][4] = malloc(sizeof(attackunit_t));
	*mapa[3][4] = *P(artillery, 57, 1);
	mapa[254][254] = malloc(sizeof(attackunit_t));
	*mapa[254][254] = *P(battleship, 99, 1);

	attackunit_t* compartida = malloc(sizeof(attackunit_t));
	*compartida = *P(anti-air, 60, 0);

	mapa_t esperado = {0};
	esperado[0][0] = compartida;
	esperado[43][89] = compartida;
	esperado[100][100] = compartida;
	esperado[201][10] = compartida;
	esperado[241][241] = compartida;
	esperado[4][3] = mapa[4][3];
	esperado[3][4] = mapa[3][4];
	esperado[254][254] = mapa[254][254];

	optimizar(mapa, compartida, hash);
	testResults1A(esperado, mapa, compartida->references == 5, "test_1a_mapa_completo");

	free(compartida);
	free(mapa[3][4]);
	free(mapa[4][3]);
	free(mapa[254][254]);
}

/**
 * Evalúa los tests del ejercicio 1A. Este ejercicio requiere implementar
 * `es_indice_ordenado`.
 *
 * En caso de que se quieran skipear los tests alcanza con asignarle `false`
 * a `EJERCICIO_1A_HECHO`.
 */
void test_ej1a(void) {
	uint64_t successful_at_start = successful_tests;
	uint64_t failed_at_start = failed_tests;
	if (!EJERCICIO_1A_HECHO) {
		printf(SKIP "El ejercicio 1A no está hecho aún.\n");
		return;
	}

	test_1a_sin_cambios();
	test_1a_optimizo_inst_optimizada();
    test_1a_varios_cambios();
	test_1a_instancia_compartida_nueva();
	test_1a_mapa_completo(successful_at_start);

	if (failed_at_start < failed_tests) {
		printf(FAIL "El ejercicio 1A tuvo tests que fallaron.\n");
	}
}

void testResults1B(uint32_t esperado, uint32_t resultado, mapa_t mapa, char* testName) {
	bool iguales = esperado == resultado;
	if (iguales) {
		successful_tests++;
		printf(DONE "%s: la combustible total coincide.\n",testName);
	} else {
		failed_tests++;		
		printf(FAIL "%s: se contó %d de combustible pero el total correcto es %d. Mapa:\n", testName, resultado, esperado);
		print_map_combustible(mapa);
	}
}

void test_1b_varias_unidades() {
	mapa_t mapa = {0};
	mapa[0][0] = malloc(sizeof(attackunit_t));
	*mapa[0][0] = *P(anti-air, 60, 1);
	mapa[1][1] = malloc(sizeof(attackunit_t));
	*mapa[1][1] = *P(anti-air, 65, 1);
	mapa[4][3] = malloc(sizeof(attackunit_t));
	*mapa[4][3] = *P(artillery, 52, 1);
	mapa[3][4] = malloc(sizeof(attackunit_t));
	*mapa[3][4] = *P(anti-air, 60, 1);
	mapa[4][4] = malloc(sizeof(attackunit_t));
	*mapa[4][4] = *P(battleship, 102, 1);

	uint32_t obtenido = contarCombustibleAsignado(mapa, get_combustible);
	testResults1B(10, obtenido, mapa, "test_1b_varias_unidades");

	free(mapa[0][0]);
	free(mapa[1][1]);
	free(mapa[4][3]);
	free(mapa[3][4]);
	free(mapa[4][4]);
}

void test_1b_valores_base() {
	mapa_t mapa = {0};
	mapa[0][0] = malloc(sizeof(attackunit_t));
	*mapa[0][0] = *P(anti-air, 60, 1);
	mapa[1][1] = malloc(sizeof(attackunit_t));
	*mapa[1][1] = *P(mech, 70, 1);
	mapa[4][3] = malloc(sizeof(attackunit_t));
	*mapa[4][3] = *P(artillery, 50, 1);
	mapa[3][4] = malloc(sizeof(attackunit_t));
	*mapa[3][4] = *P(anti-air, 60, 1);
	mapa[4][4] = malloc(sizeof(attackunit_t));
	*mapa[4][4] = *P(battleship, 99, 1);

	uint32_t obtenido = contarCombustibleAsignado(mapa, get_combustible);
	testResults1B(0, obtenido, mapa, "test_1b_valores_base");

	free(mapa[0][0]);
	free(mapa[1][1]);
	free(mapa[4][3]);
	free(mapa[3][4]);
	free(mapa[4][4]);
}

void test_1b_ninguna_unidad() {
	mapa_t mapa = {0};

	uint32_t obtenido = contarCombustibleAsignado(mapa, get_combustible);
	testResults1B(0, obtenido, mapa, "test_1b_ninguna_unidad");
}

void test_1b_una_unidad_agrega() {
	mapa_t mapa = {0};
	mapa[4][4] = malloc(sizeof(attackunit_t));
	*mapa[4][4] = *P(battleship, 123, 1);

	uint32_t obtenido = contarCombustibleAsignado(mapa, get_combustible);
	testResults1B(24, obtenido, mapa, "test_1b_una_unidad_agrega");

	free(mapa[4][4]);
}

void test_1b_mapa_completo(uint64_t successful_at_start) {
	if (successful_tests - successful_at_start < 4) {
		printf(FAIL "El test sobre el mapa completo se ejecutará cuando pasen los tests anteriores.\n");
		return;
	}
	mapa_t mapa = {0};
	mapa[0][0] = malloc(sizeof(attackunit_t));
	*mapa[0][0] = *P(anti-air, 60, 1);
	mapa[43][89] = malloc(sizeof(attackunit_t));
	*mapa[43][89] = *P(mech, 78, 1);
	mapa[100][100] = malloc(sizeof(attackunit_t));
	*mapa[100][100] = *P(anti-air, 65, 1);
	mapa[201][10] = malloc(sizeof(attackunit_t));
	*mapa[201][10] = *P(anti-air, 60, 1);
	mapa[241][241] = malloc(sizeof(attackunit_t));
	*mapa[241][241] = *P(anti-air, 72, 1);
	mapa[4][3] = malloc(sizeof(attackunit_t));
	*mapa[4][3] = *P(artillery, 50, 1);
	mapa[3][4] = malloc(sizeof(attackunit_t));
	*mapa[3][4] = *P(artillery, 51, 1);
	mapa[254][254] = malloc(sizeof(attackunit_t));
	*mapa[254][254] = *P(battleship, 103, 1);

	uint32_t obtenido = contarCombustibleAsignado(mapa, get_combustible);
	testResults1B(8+5+12+1+4, obtenido, mapa, "test_1b_mapa_completo");

	free(mapa[0][0]);
	free(mapa[43][89]);
	free(mapa[100][100]);
	free(mapa[201][10]);
	free(mapa[241][241]);
	free(mapa[4][3]);
	free(mapa[3][4]);
	free(mapa[254][254]);
}

/**
 * Evalúa los tests del ejercicio 1B. Este ejercicio requiere implementar
 * `es_indice_ordenado`.
 *
 * En caso de que se quieran skipear los tests alcanza con asignarle `false`
 * a `EJERCICIO_1B_HECHO`.
 */
void test_ej1b(void) {
	uint64_t successful_at_start = successful_tests;
	uint64_t failed_at_start = failed_tests;
	if (!EJERCICIO_1B_HECHO) {
		printf(SKIP "El ejercicio 1B no está hecho aún.\n");
		return;
	}

	test_1b_valores_base();
	test_1b_varias_unidades();
	test_1b_ninguna_unidad();
	test_1b_una_unidad_agrega();
	test_1b_mapa_completo(successful_at_start);

	if (failed_at_start < failed_tests) {
		printf(FAIL "El ejercicio 1B tuvo tests que fallaron.\n");
	}
}

uint32_t hash_test(attackunit_t* p) {
	if (p == 0)
		return 0;
    uint32_t res = p->combustible;
    if (strcmp(p->clase, "anti-air") == 0) {
        res += 1000;
    } else if (strcmp(p->clase, "artillery") == 0) {
        res += 2000;
    } else if (strcmp(p->clase, "battleship") == 0) {
        res += 3000;
    } else if (strcmp(p->clase, "mech") == 0) {
        res += 4000;
    }
    return res;
}

void testResults1C(mapa_t esperado, mapa_t resultado, bool cantidad_refs_correcta, char* testName) {
	bool iguales = cantidad_refs_correcta;
	uint32_t i;
	for (i=0; iguales && i<255*255; i++) {
		iguales = iguales && (hash_test(((attackunit_t**) esperado)[i]) == hash_test(((attackunit_t**) resultado)[i]));
	}

	if (iguales) {
		successful_tests++;
		printf(DONE "%s: el mapa esperado y el obtenido coinciden.\n",testName);
	} else {
		failed_tests++;		
		if (!cantidad_refs_correcta) {
			printf(FAIL "%s: cantidad de referencias incorrecta.\n", testName);
		} else {
			printf(FAIL "%s: el mapa esperado y el obtenido no coinciden.\n", testName);
		}
		printf("Se esperaba un mapa con las características:\n");
		print_map(esperado);
		printf("Pero se obtuvo el siguiente mapa:\n");
		print_map(resultado);
	}
}

void test_1c_cambio_compartida() {
	mapa_t mapa = {0};
	mapa[0][0] = malloc(sizeof(attackunit_t));
	*mapa[0][0] = *P(anti-air, 60, 2);
	mapa[1][1] = mapa[0][0];
	mapa[4][3] = malloc(sizeof(attackunit_t));
	*mapa[4][3] = *P(artillery, 50, 1);
	mapa[3][4] = malloc(sizeof(attackunit_t));
	*mapa[3][4] = *P(mech, 70, 1);
	mapa[4][4] = malloc(sizeof(attackunit_t));
	*mapa[4][4] = *P(battleship, 99, 1);

	mapa_t esperado = {0};
	memcpy(esperado, mapa, sizeof(mapa_t));
	esperado[1][1] = malloc(sizeof(attackunit_t));
	*esperado[1][1] = *P(anti-air, 50, 1);

	modificarUnidad(mapa, 1, 1, fun_modificar);
	testResults1C(esperado, mapa, mapa[0][0]->references == 1,"test_1c_cambio_compartida");

	free(mapa[0][0]);
	free(mapa[1][1]);
	free(esperado[1][1]);
	free(mapa[3][4]);
	free(mapa[4][3]);
	free(mapa[4][4]);
}

void test_1c_cambio_individual() {
	mapa_t mapa = {0};
	mapa[0][0] = malloc(sizeof(attackunit_t));
	*mapa[0][0] = *P(anti-air, 60, 2);
	mapa[1][1] = mapa[0][0];
	mapa[4][3] = malloc(sizeof(attackunit_t));
	*mapa[4][3] = *P(artillery, 50, 1);
	mapa[3][4] = malloc(sizeof(attackunit_t));
	*mapa[3][4] = *P(mech, 70, 1);
	mapa[4][4] = malloc(sizeof(attackunit_t));
	*mapa[4][4] = *P(battleship, 99, 1);

	mapa_t esperado = {0};
	memcpy(esperado, mapa, sizeof(mapa_t));
	esperado[3][4] = malloc(sizeof(attackunit_t));
	*esperado[3][4] = *P(mech, 50, 1);

	modificarUnidad(mapa, 3, 4, fun_modificar);
	testResults1C(esperado, mapa, mapa[3][4]->references == 1,"test_1c_cambio_individual");

	free(mapa[0][0]);
	free(mapa[3][4]);
	free(esperado[3][4]);
	free(mapa[4][3]);
	free(mapa[4][4]);
}

/**
 * Evalúa los tests del ejercicio 1B. Este ejercicio requiere implementar
 * `es_indice_ordenado`.
 *
 * En caso de que se quieran skipear los tests alcanza con asignarle `false`
 * a `EJERCICIO_1C_HECHO`.
 */
void test_ej1c(void) {
	uint64_t successful_at_start = successful_tests;
	uint64_t failed_at_start = failed_tests;
	if (!EJERCICIO_1C_HECHO) {
		printf(SKIP "El ejercicio 1C no está hecho aún.\n");
		return;
	}

	test_1c_cambio_compartida();
	test_1c_cambio_individual();
	//test_1c_mapa_completo(successful_at_start);

	if (failed_at_start < failed_tests) {
		printf(FAIL "El ejercicio 1C tuvo tests que fallaron.\n");
	}
}

/**
 * Corre los tests de este ejercicio.
 *
 * Las variables `EJERCICIO_1A_HECHO`, `EJERCICIO_1B_HECHO` y `EJERCICIO_1C_HECHO` controlan qué
 * testsuites se van a correr. Ponerlas como `false` indica que el ejercicio no
 * está implementado y por lo tanto no querés que se corran los tests asociados
 * a él.
 *
 * Recordá que los dos ejercicios pueden implementarse independientemente uno
 * del otro.
 *
 * Si algún test falla el programa va a terminar con un código de error.
 */
int main(int argc, char* argv[]) {
	// 1A
	test_ej1a();
	// 1B
	test_ej1b();
	// 1C
	test_ej1c();

	printf(
		"\nSe corrieron %ld tests. %ld corrieron exitosamente. %ld fallaron.\n",
		failed_tests + successful_tests, successful_tests, failed_tests
	);

	if (failed_tests) {
		return 1;
	} else {
		return 0;
	}
}
