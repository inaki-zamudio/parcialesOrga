#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include "ej2.h"

/**
 * Forma de especificarle a `load_image` que no nos importa la cantidad de
 * canales de una imagen.
 */
#define ANY_CHANNEL_COUNT 0
/**
 * Cantidad de canales en un mapa de profundidad
 */
#define DEPTH_CHANNEL_COUNT 1
/**
 * Cantidad de canales en una imagen RGBA
 */
#define RGBA_CHANNEL_COUNT 4

/**
 * Una imagen.
 *
 * Campos:
 *   - width:                  El ancho de la imagen. Es positivo.
 *   - height:                 El alto de la imagen. Es positivo.
 *   - original_channel_count: La cantidad de canales de la imagen fuente.
 *   - channel_count:          La cantidad de canales (de 8 bits) cargados a
 *                             memoria.
 *   - data:                   Un buffer de (width * height * channel_count)
 *                             bytes que tiene los píxeles de la imagen.
 */
typedef struct img {
	int width;
	int height;
	int original_channel_count;
	int channel_count;
	uint8_t* data;
} img_t;

/**
 * Carga una imagen.
 *
 * Parámetros:
 *   - filepath:         Ruta del sistema de archivos en dónde se encuentra la
 *                       imagen.
 *   - desired_channels: Cantidad de canales a esperar encontrar en la imagen.
 *                       Cero representa _"no sé"_.
 */
static img_t load_image(const char* filepath, int desired_channels) {
	img_t result;
	result.data = stbi_load(
		filepath,
		&result.width,
		&result.height,
		&result.original_channel_count,
		desired_channels
	);
	result.channel_count = desired_channels;
	return result;
}

/**
 * Convierte un mapa de profundidad de 8 bits sin signo a 32 bits con signo.
 *
 * Parámetros:
 *   - dst:    Imagen destino. 32 bits con signo por píxel.
 *   - src:    Imagen fuente. 8 bits sin signo por píxel.
 *   - width:  Ancho de la imagen. Debe ser positivo.
 *   - height: Alto de la imagen. Debe ser positivo.
 */
static void depth_convert_uint8_to_int32(
	int32_t* dst,
	uint8_t* src,
	uint32_t width, uint32_t height
) {
	for (uint32_t y = 0; y < height; y++) {
		for (uint32_t x = 0; x < width; x++) {
			uint32_t i = y * width + x;
			dst[i] = src[i];
		}
	}
}

/**
 * Tamaño máximo soportado para rutas del sistema de archivos
 */
#define MAX_FILEPATH_SIZE 127

/**
 * Construye una ruta en el sistema operativo por medio de agregarle un prefijo
 * y un sufijo a un nombre dado.
 *
 * Parámetros:
 *   - filepath: Buffer en dónde escribir el resultado.
 *   - prefix:   Texto a poner al principio de la ruta.
 *   - name:     Texto a poner en el medio de la ruta. Es algún tipo de nombre.
 *   - suffix:   Texto a poner al final de la ruta. Normalmente es una
 *               extensión.
 */
void build_filepath(char filepath[MAX_FILEPATH_SIZE + 1], const char* prefix, const char* name, const char* suffix) {
	filepath[0] = 0;
	strncat(filepath, prefix, MAX_FILEPATH_SIZE);
	strncat(filepath, name, MAX_FILEPATH_SIZE);
	strncat(filepath, suffix, MAX_FILEPATH_SIZE);
}

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
 * El mensaje [SKIP] escrito en magenta.
 */
#define SKIP "[\033[95;1mSKIP\033[0m] "

/**
 * Compara un resultado con la imagen esperada. Devuelve `true` si encuentra
 * diferencias.
 *
 * Parámetros:
 *   - name:        Nombre del test.
 *   - buffer:      Resultado a comparar.
 *   - buffer_size: Tamaño en bytes del resultado a comparar.
 */
bool has_differences(const char* name, void* buffer, uint64_t buffer_size) {
	bool result = false;
	char filepath[MAX_FILEPATH_SIZE + 1];
	build_filepath(filepath, "expected/", name, ".png");

	img_t expected = load_image(filepath, ANY_CHANNEL_COUNT);
	uint64_t expected_data_size = expected.width * expected.height * expected.original_channel_count;

	if (buffer_size != expected_data_size) {
		result = true;
	} else if (memcmp(buffer, expected.data, buffer_size) != 0) {
		result = true;
	}

	stbi_image_free(expected.data);
	return result;
}

/**
 * Corre un test del ejercicio 2A.
 *
 * Parámetros:
 *   - name_out: Nombre del test.
 *   - name:     Nombre de la imagen de entrada.
 *   - scale:    Factor de escalado.
 *   - offset:   Factor de corrimiento.
 */
void do_test_a(
	const char* name_out,
	const char* name, int32_t scale, int32_t offset
) {
	uint64_t failed_at_start = failed_tests;
	if (!EJERCICIO_2A_HECHO) {
		printf(SKIP "El ejercicio 2A no está hecho aún.\n");
		return;
	}

	char filepath[MAX_FILEPATH_SIZE + 1];
	build_filepath(filepath, "img/depth/", name, ".png");

	char filepath_out[MAX_FILEPATH_SIZE + 1];
	build_filepath(filepath_out, "outputs/", name_out, ".png");

	img_t depth = load_image(filepath, DEPTH_CHANNEL_COUNT);

	uint32_t width = depth.width;
	uint32_t height = depth.height;

	uint64_t depth_fixed_size = width * height * sizeof(int32_t);
	int32_t* depth_fixed = malloc(depth_fixed_size);

	ej2a(depth_fixed, depth.data, scale, offset, width, height);
	// No existe PNG con 32 bits por canal. Entonces vamos a escribirlo como
	// escala de grises pero 4X el ancho.
	stbi_write_png(
		filepath_out,
		width * sizeof(int32_t), height,
		DEPTH_CHANNEL_COUNT,
		depth_fixed,
		width * sizeof(int32_t)
	);
	if (has_differences(name_out, depth_fixed, depth_fixed_size)) {
		printf(FAIL "do_test_a(\"%s\", \"%s\", %d, %d)\n", name_out, name, scale, offset);
		printf(FAIL "  Se encontraron diferencias:\n");
		printf(FAIL "    Imagen de entrada: %s\n", filepath);
		printf(FAIL "    Escala: %d\n", scale);
		printf(FAIL "    Offset: %d\n", offset);
		printf(FAIL "    Imagen de salida: %s\n", filepath_out);
		printf(FAIL "    Imagen de salida esperada: expected/%s.png\n", name_out);
		printf(FAIL "\n");
		failed_tests++;
	} else {
		printf(DONE "do_test_a(\"%s\", \"%s\", %d, %d)\n", name_out, name, scale, offset);
		successful_tests++;
	}

	free(depth_fixed);
	stbi_image_free(depth.data);
}

/**
 * Corre un test del ejercicio 2B.
 *
 * Parámetros:
 *   - name_out: Nombre del test.
 *   - name_a:   Nombre de la imagen de entrada A.
 *   - name_b:   Nombre de la imagen de entrada B.
 */
void do_test_b(
	const char* name_out,
	const char* name_a,
	const char* name_b
) {
	uint64_t failed_at_start = failed_tests;
	if (!EJERCICIO_2B_HECHO) {
		printf(SKIP "El ejercicio 2B no está hecho aún.\n");
		return;
	}

	char filepath_a[MAX_FILEPATH_SIZE + 1];
	build_filepath(filepath_a, "img/", name_a, ".png");

	char filepath_b[MAX_FILEPATH_SIZE + 1];
	build_filepath(filepath_b, "img/", name_b, ".png");

	char depth_filepath_a[MAX_FILEPATH_SIZE + 1];
	build_filepath(depth_filepath_a, "img/depth/", name_a, ".png");

	char depth_filepath_b[MAX_FILEPATH_SIZE + 1];
	build_filepath(depth_filepath_b, "img/depth/", name_b, ".png");

	char filepath_out[MAX_FILEPATH_SIZE + 1];
	build_filepath(filepath_out, "outputs/", name_out, ".png");

	img_t a = load_image(filepath_a, RGBA_CHANNEL_COUNT);
	img_t b = load_image(filepath_b, RGBA_CHANNEL_COUNT);
	img_t depth_a = load_image(depth_filepath_a, DEPTH_CHANNEL_COUNT);
	img_t depth_b = load_image(depth_filepath_b, DEPTH_CHANNEL_COUNT);

	assert(a.width == b.width && a.width == depth_a.width && a.width == depth_b.width);
	assert(a.height == b.height && a.height == depth_a.height && a.height == depth_b.height);

	uint32_t width = a.width;
	uint32_t height = a.height;
	uint64_t out_size = width * height * sizeof(rgba_t);
	rgba_t* out = malloc(out_size);
	int32_t* depth_a_fixed = malloc(width * height * sizeof(int32_t));
	int32_t* depth_b_fixed = malloc(width * height * sizeof(int32_t));

	depth_convert_uint8_to_int32(depth_a_fixed, depth_a.data, width, height);
	depth_convert_uint8_to_int32(depth_b_fixed, depth_b.data, width, height);
	ej2b(
		out,
		(rgba_t*) a.data, depth_a_fixed,
		(rgba_t*) b.data, depth_b_fixed,
		width, height
	);
	stbi_write_png(
		filepath_out,
		width, height,
		RGBA_CHANNEL_COUNT,
		out,
		width * sizeof(rgba_t)
	);
	if (has_differences(name_out, out, out_size)) {
		printf(FAIL "do_test_b(\"%s\", \"%s\", \"%s\")\n", name_out, name_a, name_b);
		printf(FAIL "  Se encontraron diferencias:\n");
		printf(FAIL "    Imagen de entrada A: %s\n", filepath_a);
		printf(FAIL "    Imagen de entrada B: %s\n", filepath_b);
		printf(FAIL "    Mapa de profundidad A: %s\n", depth_filepath_a);
		printf(FAIL "    Mapa de profundidad B: %s\n", depth_filepath_b);
		printf(FAIL "    Imagen de salida: %s\n", filepath_out);
		printf(FAIL "    Imagen de salida esperada: expected/%s.png\n", name_out);
		printf(FAIL "\n");
		failed_tests++;
	} else {
		printf(DONE "do_test_b(\"%s\", \"%s\", \"%s\")\n", name_out, name_a, name_b);
		successful_tests++;
	}

	free(out);
	free(depth_a_fixed);
	free(depth_b_fixed);
	stbi_image_free(a.data);
	stbi_image_free(b.data);
	stbi_image_free(depth_a.data);
	stbi_image_free(depth_b.data);
}

/**
 * Corre un test que ejercita tanto el ejercicio 2A como el 2B.
 *
 * Parámetros:
 *   - name_out: Nombre del test.
 *   - name_a:   Nombre de la imagen de entrada A.
 *   - scale_a:  Factor de escalado de la entrada A.
 *   - offset_a: Factor de corrimiento de la entrada A.
 *   - name_b:   Nombre de la imagen de entrada B.
 *   - scale_b:  Factor de escalado de la entrada B.
 *   - offset_b: Factor de corrimiento de la entrada B.
 */
static void do_test_ab(
	const char* name_out,
	const char* name_a, int32_t scale_a, int32_t offset_a,
	const char* name_b, int32_t scale_b, int32_t offset_b
) {
	char filepath_a[MAX_FILEPATH_SIZE + 1];
	build_filepath(filepath_a, "img/", name_a, ".png");

	char filepath_b[MAX_FILEPATH_SIZE + 1];
	build_filepath(filepath_b, "img/", name_b, ".png");

	char depth_filepath_a[MAX_FILEPATH_SIZE + 1];
	build_filepath(depth_filepath_a, "img/depth/", name_a, ".png");

	char depth_filepath_b[MAX_FILEPATH_SIZE + 1];
	build_filepath(depth_filepath_b, "img/depth/", name_b, ".png");

	char filepath_out[MAX_FILEPATH_SIZE + 1];
	build_filepath(filepath_out, "outputs/", name_out, ".png");

	img_t a = load_image(filepath_a, RGBA_CHANNEL_COUNT);
	img_t b = load_image(filepath_b, RGBA_CHANNEL_COUNT);
	img_t depth_a = load_image(depth_filepath_a, DEPTH_CHANNEL_COUNT);
	img_t depth_b = load_image(depth_filepath_b, DEPTH_CHANNEL_COUNT);

	assert(a.width == b.width && a.width == depth_a.width && a.width == depth_b.width);
	assert(a.height == b.height && a.height == depth_a.height && a.height == depth_b.height);

	uint32_t width = a.width;
	uint32_t height = a.height;
	uint64_t out_size = width * height * sizeof(rgba_t);
	rgba_t* out = malloc(out_size);
	int32_t* depth_a_fixed = malloc(width * height * sizeof(int32_t));
	int32_t* depth_b_fixed = malloc(width * height * sizeof(int32_t));

	ej2a(depth_a_fixed, depth_a.data, scale_a, offset_a, width, height);
	ej2a(depth_b_fixed, depth_b.data, scale_b, offset_b, width, height);
	ej2b(
		out,
		(rgba_t*) a.data, depth_a_fixed,
		(rgba_t*) b.data, depth_b_fixed,
		width, height
	);
	stbi_write_png(
		filepath_out,
		width, height,
		RGBA_CHANNEL_COUNT,
		out,
		width * sizeof(rgba_t)
	);
	if (has_differences(name_out, out, out_size)) {
		printf(
			FAIL "do_test_ab(\"%s\", \"%s\", %d, %d, \"%s\", %d, %d)\n",
			name_out, name_a, scale_a, offset_a, name_b, scale_b, offset_b
		);
		printf(FAIL "  Se encontraron diferencias:\n");
		printf(FAIL "    Imagen de entrada A: %s\n", filepath_a);
		printf(FAIL "    Imagen de entrada B: %s\n", filepath_b);
		printf(FAIL "    Mapa de profundidad A: %s\n", depth_filepath_a);
		printf(FAIL "    Mapa de profundidad B: %s\n", depth_filepath_b);
		printf(FAIL "    Escala A: %d\n", scale_a);
		printf(FAIL "    Offset A: %d\n", offset_a);
		printf(FAIL "    Escala B: %d\n", scale_b);
		printf(FAIL "    Offset B: %d\n", offset_b);
		printf(FAIL "    Imagen de salida: %s\n", filepath_out);
		printf(FAIL "    Imagen de salida esperada: expected/%s.png\n", name_out);
		printf(FAIL "\n");
		failed_tests++;
	} else {
		printf(
			DONE "do_test_ab(\"%s\", \"%s\", %d, %d, \"%s\", %d, %d)\n",
			name_out, name_a, scale_a, offset_a, name_b, scale_b, offset_b
		);
		successful_tests++;
	}

	free(out);
	free(depth_a_fixed);
	free(depth_b_fixed);
	stbi_image_free(a.data);
	stbi_image_free(b.data);
	stbi_image_free(depth_a.data);
	stbi_image_free(depth_b.data);

}

void test_ej2a(void) {
	uint64_t failed_at_start = failed_tests;
	if (!EJERCICIO_2A_HECHO) {
		printf(SKIP "El ejercicio 2A no está hecho aún.\n");
		return;
	}

	do_test_a("a_1", "grim",              4,  100);
	do_test_a("a_2", "grim/SP_0_WEBWS", 200,   -5);
	do_test_a("a_3", "grim/me_0_carla",  -1,  255);
	do_test_a("a_4", "grim/mt_1_diats",   3, -164);

	if (failed_at_start < failed_tests) {
		printf(FAIL "El ejercicio 2A tuvo tests que fallaron.\n");
	}
}

/**
 * Evalúa los tests del ejercicio 2B. Este ejercicio requiere implementar
 * `ej2b`.
 *
 * En caso de que se quieran skipear los tests alcanza con asignarle `false`
 * a `EJERCICIO_2B_HECHO`.
 */
void test_ej2b(void) {
	uint64_t failed_at_start = failed_tests;
	if (!EJERCICIO_2B_HECHO) {
		printf(SKIP "El ejercicio 2B no está hecho aún.\n");
		return;
	}

	do_test_b("b_1", "martini1841",     "mascotas");
	do_test_b("b_2", "grim",            "grim-flip");
	do_test_b("b_3", "grim/me_0_carla", "grim/me_1_pmpws");
	do_test_b("b_4", "grim/re_1_front", "grim/jb_0_garin");
	do_test_b("b_5", "grim/ei_1_intha", "grim/ei_1_intha-flip");

	if (failed_at_start < failed_tests) {
		printf(FAIL "El ejercicio 2B tuvo tests que fallaron.\n");
	}
}

/**
 * Corre tests que ejercitan tanto el ejercicio 2A como el 2B.
 *
 * En caso de que se quieran skipear los tests alcanza con asignarle `false`
 * a `EJERCICIO_2A_HECHO` ó a `EJERCICIO_2B_HECHO`.
 */
void test_ej2ab(void) {
	uint64_t failed_at_start = failed_tests;
	if (!EJERCICIO_2A_HECHO) {
		printf(SKIP "El ejercicio 2A no está hecho aún. No se correrá el test integrador.\n");
		return;
	}
	if (!EJERCICIO_2B_HECHO) {
		printf(SKIP "El ejercicio 2B no está hecho aún. No se correrá el test integrador.\n");
		return;
	}

	do_test_ab("ab_1", "grim/ly_2_sltha",  2,     100, "grim/mt_1_diats",   3, -164);
	do_test_ab("ab_2", "grim/me_0_carla", -1,     255, "grim/mt_1_diats",   1, -139);
	do_test_ab("ab_3", "grim/re_1_front",  1, 256*208, "grim/jb_0_garin", 256,    0);
	do_test_ab("ab_4", "grim/dr_0_winws", -1,       0, "grim/dr_1_dorrv",  -1,    0);
	do_test_ab("ab_5", "grim/ci_0_stpws", -1,       0, "grim/hl_0_dorrv",  -1,    0);
	do_test_ab("ab_6", "grim/tw_0_trkvw", -1,       0, "grim/le_2_ladws",  -1,    0);
	do_test_ab("ab_7", "grim/SP_1_ENTLA",  2,    -112, "grim/tr_2_pmpws",  -1,  255);
	do_test_ab("ab_8", "grim/co_3_dskrv", -1,     255, "nada",              1,  100);
	do_test_ab("ab_9", "grim/ce_2_front", -1,     255, "nada",              1,  240);

	if (failed_at_start < failed_tests) {
		printf(FAIL "EjAB tuvo tests que fallaron.\n");
	}
}

/**
 * Corre los tests de este ejercicio.
 *
 * Las variables `EJERCICIO_2A_HECHO` y `EJERCICIO_2B_HECHO` controlan qué
 * testsuites se van a correr. Ponerlas como `false` indica que el ejercicio no
 * está implementado y por lo tanto no querés que se corran los tests
 * asociados a él.
 *
 * Recordá que el ejercicio 2A se puede implementar independientemente del 2B.
 *
 * Si algún test falla el programa va a terminar con un código de error.
 */
int main(int argc, char* argv[]) {
	// 2A
	test_ej2a();
	// 2B
	test_ej2b();
	// 2A y 2B
	test_ej2ab();

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
