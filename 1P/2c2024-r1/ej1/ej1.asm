extern malloc
extern free

section .rodata
; Acá se pueden poner todas las máscaras y datos que necesiten para el ejercicio

section .text
; Marca un ejercicio como aún no completado (esto hace que no corran sus tests)
FALSE EQU 0
; Marca un ejercicio como hecho
TRUE  EQU 1

FILAS EQU 255
COLUMNAS EQU 255

; Marca el ejercicio 1A como hecho (`true`) o pendiente (`false`).
;
; Funciones a implementar:
;   - optimizar
global EJERCICIO_1A_HECHO
EJERCICIO_1A_HECHO: db TRUE ; Cambiar por `TRUE` para correr los tests.

; Marca el ejercicio 1B como hecho (`true`) o pendiente (`false`).
;
; Funciones a implementar:
;   - contarCombustibleAsignado
global EJERCICIO_1B_HECHO
EJERCICIO_1B_HECHO: db FALSE ; Cambiar por `TRUE` para correr los tests.

; Marca el ejercicio 1C como hecho (`true`) o pendiente (`false`).
;
; Funciones a implementar:
;   - modificarUnidad
global EJERCICIO_1C_HECHO
EJERCICIO_1C_HECHO: db FALSE ; Cambiar por `TRUE` para correr los tests.

;########### ESTOS SON LOS OFFSETS Y TAMAÑO DE LOS STRUCTS
; Completar las definiciones (serán revisadas por ABI enforcer):
ATTACKUNIT_CLASE EQU 0
ATTACKUNIT_COMBUSTIBLE EQU 12
ATTACKUNIT_REFERENCES EQU 14
ATTACKUNIT_SIZE EQU 16

global optimizar
optimizar:
	; Te recomendamos llenar una tablita acá con cada parámetro y su
	; ubicación según la convención de llamada. Prestá atención a qué
	; valores son de 64 bits y qué valores son de 32 bits o 8 bits.
	;
	; rdi = mapa_t           mapa
	; rsi = attackunit_t*    compartida
	; rdx = uint32_t*        fun_hash(attackunit_t*)

	; prologo
	push rbp
	mov rbp, rsp
	push r12
	push r13
	push r14
	push r15
	push rbx
	sub rsp, 8 ; pila alineada

	mov r12, rdi ; mapa
	mov r13, rsi ; compartida
	mov r14, rdx ; fun_hash

	mov rdi, r13 ; preparo compartida para el call
	call r14 ; eax <-- hash_compartida
	mov r15d, eax ; r15d = hash_compartida

	xor rbx, rbx ; int i = 0
.loop:
	cmp rbx, FILAS * COLUMNAS ; ¿ i == 255^2 ?
	jge .fin

	cmp r12, r13 ; if (mapa[i][j] == compartida)
	je .nextIteration

	cmp r12, 0 ; if (mapa[i][j] == NULL)
	je .nextIteration

	mov rdi, r12 ; preparo mapa[i][j] para call
	call r14 ; eax <-- hash_actual = fun_hash(mapa[i][j])

	cmp eax, r15d ; if(hash_compartida != hash_actual)
	jne .nextIteration

	dec byte [r12 + ATTACKUNIT_REFERENCES] ; mapa[i][j]->references--

	cmp byte [r12 + ATTACKUNIT_REFERENCES], 0
	je .liberar

.nextIteration:
	add r12, 8 ; mapa[(i*j)+1]
	inc rbx ; (i++)
	jmp .loop

.liberar:
	mov rdi, r12 ; preparo mapa[i][j] para call
	call free
	mov [r12], r13 ; mapa[i][j] = compartida
	inc byte [r13 + ATTACKUNIT_REFERENCES] ; compartida->references++
	jmp .nextIteration

.fin:
	; epilogo
	add rsp, 8 ; restauro pila
	pop rbx
	pop r15
	pop r14
	pop r13
	pop r12
	pop rbp
	ret
	
global contarCombustibleAsignado
contarCombustibleAsignado:
	; rdi = mapa_t           mapa
	; rsi = uint16_t*        fun_combustible(char*)
	ret
	
global modificarUnidad
modificarUnidad:
	; rdi = mapa_t           mapa
	; sil  = uint8_t          x
	; dil  = uint8_t          y
	; rcx = void*            fun_modificar(attackunit_t*)
	ret