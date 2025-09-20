AS = nasm
ASFLAGS = -felf64 -Fdwarf -g
LDFLAGS = -lm
CFLAGS= -Wall -Wextra -Wno-pedantic -O0 -g -Wno-unused-function -Wno-unused-variable -Wno-unused-parameter -no-pie -z noexecstack -I ../../test_utils/
VPATH = .:../test_utils
STRUCTS_FILE = ejercicio.h
STRUCTS_PATH = "../src"

# Crea los binarios en cuestión
all: test_c test_asm test_abi

# Corre los tests usando la implementación en C
run_c: test_c
	./test_c

# Corre los tests usando la implementación en ASM
run_asm: test_asm
	./test_asm

# Corre los tests usando la implementación en ASM instrumentada mediante el ABI enforcer
run_abi: test_abi
	./test_abi

# Corre los tests en valgrind usando la implementación en C
valgrind_c: test_c
	valgrind  --show-reachable=yes --leak-check=full --error-exitcode=1 ./test_c \
	&& echo "No se detectaron errores de memoria"

# Corre los tests en valgrind usando la implementación en ASM
valgrind_asm: test_asm
	valgrind  --show-reachable=yes --leak-check=full --error-exitcode=1 ./test_asm \
	&& echo "No se detectaron errores de memoria"

# Corre los tests en valgrind usando la implementación en ASM instrumentada mediante el ABI enforcer
valgrind_abi: test_abi
	valgrind  --show-reachable=yes --leak-check=full --error-exitcode=1 ./test_abi \
	&& echo "No se detectaron errores de memoria ni ABI"

# Borra todos los archivos generados
clean:
	rm -f test_c test_asm test_abi test_abi_ej1 test_abi_ej2 test_abi_ej3 *.o
	@$(MAKE) -C ../test_utils clean

test_c: test.c solucion.o
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

test_asm: test.c solucion_asm.o
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

test_abi: test.c solucion_abi.o abi_enforcer_regsave.o
	$(CC) $(CFLAGS) -DWITH_ABI_ENFORCER=1 $^ -o $@ $(LDFLAGS)

solucion_asm.o: solucion.asm
	$(AS) $(ASFLAGS) $^ -o $@

solucion_abi.o: solucion.asm
	$(AS) $(ASFLAGS) -p "../test_utils/abi_enforcer.mac" $< -o $@

check_offsets:
	@$(MAKE) -C ../test_utils INPUT_C=$(STRUCTS_FILE) INPUT_ASM=solucion.asm INPUT_PATH=$(STRUCTS_PATH) --no-print-directory

abi_enforcer_regsave.o: ../test_utils/abi_enforcer_regsave.asm
	$(AS) $(ASFLAGS) $< -o $@

# Estas son las entradas que usa el CI.
# Existen para poder reportar por separado el progreso de cada ejercicio.

test_abi_ej1: test.c solucion_abi.o abi_enforcer_regsave.o
	$(CC) $(CFLAGS) -DWITH_ABI_ENFORCER=1 -DEXCLUDE_EJ2 -DEXCLUDE_EJ3 $^ -o $@ $(LDFLAGS)

test_abi_ej2: test.c solucion_abi.o abi_enforcer_regsave.o
	$(CC) $(CFLAGS) -DWITH_ABI_ENFORCER=1 -DEXCLUDE_EJ1 -DEXCLUDE_EJ3 $^ -o $@ $(LDFLAGS)

test_abi_ej3: test.c solucion_abi.o abi_enforcer_regsave.o
	$(CC) $(CFLAGS) -DWITH_ABI_ENFORCER=1 -DEXCLUDE_EJ1 -DEXCLUDE_EJ2 $^ -o $@ $(LDFLAGS)

valgrind_abi_ej1: test_abi_ej1
	valgrind  --show-reachable=yes --leak-check=full --error-exitcode=1 ./test_abi_ej1 \
	&& echo "No se detectaron errores de memoria ni ABI"

valgrind_abi_ej2: test_abi_ej2
	valgrind  --show-reachable=yes --leak-check=full --error-exitcode=1 ./test_abi_ej2 \
	&& echo "No se detectaron errores de memoria ni ABI"

valgrind_abi_ej3: test_abi_ej3
	valgrind  --show-reachable=yes --leak-check=full --error-exitcode=1 ./test_abi_ej3 \
	&& echo "No se detectaron errores de memoria ni ABI"

.PHONY: clean run_c run_asm run_abi main_c main_asm valgrind_c valgrind_asm valgrind_abi check_offsets

