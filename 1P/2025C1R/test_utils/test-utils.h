#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

static size_t test__count = 0;
static size_t test__succeed__count = 0;

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

/**
 * El mensaje `OK` escrito en verde.
 */
#define OK "\033[32;1mOK\033[0m"

/**
 * El mensaje `SKIPPED` escrito en amarillo.
 */
#define SKIPPED "\033[33;1mSKIPPED\033[0m"

/**
 * El mensaje `FAILED` escrito en rojo.
 */
#define FAILED "\033[31;1mFAILED\033[0m"

extern void abienf_regsave(void*);
#if WITH_ABI_ENFORCER
static char *REGS[] = {"rbx", "rbp", "r12", "r13", "r14", "r15"};
static uint64_t abienf_regs[2][6];

static void abienf_begin() {
	abienf_regsave(abienf_regs[0]);
}

static void abienf_end(bool *failed) {
	abienf_regsave(abienf_regs[1]);
	for (size_t i=0; i<6; i++) {
		if (abienf_regs[0][i] != abienf_regs[1][i]) {
			// *failed = 1;
			printf(ANSI_COLOR_RED "\nerror: [ABI_enforcer] No se preservó el valor del registro %s." ANSI_COLOR_RESET "\n", REGS[i]);
		}
	}
}

static void* abienf_held_p;
static int   abienf_held_i;

#define TEST_CALL_V(fn, ...) (abienf_begin(), fn(__VA_ARGS__), abienf_end(test__fallo))
#define TEST_CALL_I(fn, ...) (abienf_begin(), abienf_held_i = fn(__VA_ARGS__), abienf_end(test__fallo), abienf_held_i)
#define TEST_CALL_S(fn, ...) (abienf_begin(), abienf_held_p = fn(__VA_ARGS__), abienf_end(test__fallo), abienf_held_p)
#else
#define TEST_CALL_V(fn, ...) fn(__VA_ARGS__)
#define TEST_CALL_I(fn, ...) fn(__VA_ARGS__)
#define TEST_CALL_S(fn, ...) fn(__VA_ARGS__)
#endif

#define TEST(name, enable_on)                                        \
void name##__impl(const char* test__name, bool* test__fallo, char assert_name[1024]); \
void name() {                                                        \
	test__count++;                                               \
	char assert_name[1024] = {0};                                \
	const char* test__name = #name;                              \
	bool test__fallo = false;                                    \
	printf("- %s", test__name);                                  \
	if (enable_on) {                                             \
		name##__impl(test__name, &test__fallo, assert_name); \
	} else {                                                     \
		test__fallo = true;                                  \
		printf(" " SKIPPED "\n");                            \
	}                                                            \
	if (!test__fallo) {                                          \
		test__succeed__count++;                              \
		printf(" " OK "\n");                                 \
	}                                                            \
}                                                                    \
void name##__impl(const char* test__name, bool* test__fallo, char assert_name[1024])

#define ASSERT(cond)                                                \
	if (!(cond)) {                                              \
		if (!*test__fallo) {                                \
			printf(" " FAILED "\n");                    \
		}                                                   \
		if (assert_name[0] == '\0') {                       \
			strcpy(assert_name, #cond);                 \
		}                                                   \
		printf("    al probar %s\n", assert_name);          \
		printf("    fallo en %s:%d\n", __FILE__, __LINE__); \
		printf("        Condición: %s\n", #cond);           \
		*test__fallo = true;                                \
	}                                                           \
	assert_name[0] = '\0'

#define ASSERT_EQ(type, expected, got)                              \
	if ((type)(expected) != (type)(got)) {                      \
		char format[1024];                                  \
		if (!*test__fallo) {                                \
			printf(" " FAILED "\n");                    \
		}                                                   \
		if (assert_name[0] == '\0') {                       \
			strcpy(assert_name, #expected " == " #got); \
		}                                                   \
		printf("    al probar %s\n", assert_name);          \
		printf("    fallo en %s:%d\n", __FILE__, __LINE__); \
		printf("        Esperado: ");                       \
		PRINT_VALUE(type, (type)(expected));                \
		printf("\n");                                       \
		printf("        Recibido: ");                       \
		PRINT_VALUE(type, (type)(got));                     \
		printf("\n");                                       \
		*test__fallo = true;                                \
	}                                                           \
	assert_name[0] = '\0'

/* Dumb generic printing mechanism */
static void print_int32_t(int32_t v)   { printf("%d",   v); }
static void print_uint32_t(uint32_t v) { printf("%u",   v); }
static void print_string(char* v)      { printf("%s",   v); }
static void print_float(float v)       { printf("%.2f", v); }
static void print_double(double v)     { printf("%.2f", v); }
static void print_bool(bool v)         { printf(v ? "true" : "false"); }
static void print_void_ptr(void* v)    { printf("%p", v);   }

#define PRINT_VALUE(type, value) _Generic(*((type*)NULL), \
	int32_t:  print_int32_t,         \
	uint32_t: print_uint32_t,        \
	char*:    print_string,          \
	float:    print_float,           \
	double:   print_double,          \
	bool:     print_bool,            \
	void*:    print_void_ptr         \
)(value)

static inline void tests_end(char* test_suite_name) {
	printf(
		"Pasaron %ld de %ld tests\n",
		test__succeed__count,
		test__count
	);
	if (test__count == test__succeed__count) {
		printf("¡Pasaron todos los tests de %s!\n", test_suite_name);
		exit(0);
	} else {
		printf("No pasaron todos los tests de %s.\n", test_suite_name);
		exit(1);
	}
}

void error_unaligned_stack() {
	printf(ANSI_COLOR_RED "\nerror: [ABI_enforcer] Se intentó hacer un call con la pila desalineada. Revisar el stackframe. Abortando..." ANSI_COLOR_RESET "\n");
}
