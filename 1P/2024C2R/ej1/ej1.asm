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
EJERCICIO_1B_HECHO: db TRUE ; Cambiar por `TRUE` para correr los tests.

; Marca el ejercicio 1C como hecho (`true`) o pendiente (`false`).
;
; Funciones a implementar:
;   - modificarUnidad
global EJERCICIO_1C_HECHO
EJERCICIO_1C_HECHO: db TRUE ; Cambiar por `TRUE` para correr los tests.

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
	je .fin

	cmp [r12], r13 ; if (mapa[i][j] == compartida)
	je .nextIteration

	cmp qword [r12], 0 ; if (mapa[i][j] == NULL)
	je .nextIteration

	mov rdi, [r12] ; preparo mapa[i][j] para call
	call r14 ; eax <-- hash_actual = fun_hash(mapa[i][j])

	cmp eax, r15d ; if(hash_compartida != hash_actual)
	jne .nextIteration

	mov r8, [r12]
	dec byte [r8 + ATTACKUNIT_REFERENCES] ; mapa[i][j]->references--

	cmp byte [r8 + ATTACKUNIT_REFERENCES], 0 ; if (mapa[i][j]->references == 0)
	jne .reasignar

	mov rdi, [r12] ; preparo mapa[i][j] para call
	call free
	
.reasignar:
	mov [r12], r13 ; mapa[i][j] = compartida
	inc byte [r13 + ATTACKUNIT_REFERENCES] ; compartida->references++

.nextIteration:
	add r12, 8 ; mapa[(i*j)+1]
	inc rbx ; (i++)
	jmp .loop

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
	
	; prologo
	push rbp
	mov rbp, rsp
	push r12
	push r13
	push r14
	push r15
	push rbx
	sub rsp, 8
	
	mov r12, rdi ; mapa
	mov r13, rsi ; fun_combustible

	xor r15d, r15d ; res
	xor rbx, rbx ; i

.loop:
	cmp rbx, FILAS * COLUMNAS ; i == FILAS * COLUMNAS?
	je .fin

	cmp qword [r12], 0 ; if(mapa[i][j] == NULL)
	je .nextIteration

	mov r14, [r12]
	mov rdi, [r12 + ATTACKUNIT_CLASE]
	call r13 ; ax <-- fun_combustible(mapa[i][j]->clase)

	mov r8w, word [r14 + ATTACKUNIT_COMBUSTIBLE] ; comb_base
	sub r8w, ax ; combustible - comb_base
	movzx r8d, r8w
	add r15d, r8d ; res += mapa[i][j]->combustible - comb_base

.nextIteration:
	add r12, 8
	inc rbx
	jmp .loop

.fin:
	; epilogo
	mov eax, r15d
	add rsp, 8
	pop rbx
	pop r15
	pop r14
	pop r13
	pop r12
	pop rbp
	ret
	
global modificarUnidad
modificarUnidad:
    ; rdi --> mapa 
    ; sil --> x 
    ; dl  --> y
    ; rcx --> fun_modificar

    ; prólogo
    push rbp
    mov rbp, rsp
    push r12
    push r13
    push r14
    push r15
    push rbx
    sub rsp, 8

    mov r12, rdi         ; r12 = mapa (puntero base)
    movzx r13, sil       ; r13 = x
    movzx r14, dl        ; r14 = y
    mov r15, rcx         ; r15 = fun_modificar

    mov r8, r13          ; r8 = x
    imul r8, 255         ; r8 = x * 255
    add r8, r14          ; r8 = x * 255 + y
    shl r8, 3            ; r8 = (x * 255 + y) * 8

    mov rbx, [r12 + r8]  ; rbx = mapa[x][y] (puntero a unidad)

    cmp rbx, 0
    je .fin

    cmp byte [rbx + ATTACKUNIT_REFERENCES], 1
    je .solo_fun_mod

    ; Clonar unidad
    mov rdi, ATTACKUNIT_SIZE
    call malloc          ; rax = nueva_unidad

    mov r9, rax          ; r9 = nueva_unidad

    ; Copiar toda la estructura (16 bytes)
    mov rax, [rbx]
    mov [r9], rax
    mov rax, [rbx + 8]
    mov [r9 + 8], rax

    dec byte [rbx + ATTACKUNIT_REFERENCES] ; mapa[x][y]->references--
    mov byte [r9 + ATTACKUNIT_REFERENCES], 1 ; nueva_unidad->references = 1

    mov rdi, r9
    call r15             ; fun_modificar(nueva_unidad)

    mov [r12 + r8], r9   ; mapa[x][y] = nueva_unidad

    jmp .fin

.solo_fun_mod:
    mov rdi, rbx
    call r15

.fin:
    add rsp, 8
    pop rbx
    pop r15
    pop r14
    pop r13
    pop r12
    pop rbp
    ret
