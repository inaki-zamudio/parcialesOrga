extern malloc

section .rodata
; Acá se pueden poner todas las máscaras y datos que necesiten para el ejercicio

section .text
; Marca un ejercicio como aún no completado (esto hace que no corran sus tests)
FALSE EQU 0
; Marca un ejercicio como hecho
TRUE  EQU 1

; Marca el ejercicio 1A como hecho (`true`) o pendiente (`false`).
;
; Funciones a implementar:
;   - es_indice_ordenado
global EJERCICIO_1A_HECHO
EJERCICIO_1A_HECHO: db TRUE ; Cambiar por `TRUE` para correr los tests.

; Marca el ejercicio 1B como hecho (`true`) o pendiente (`false`).
;
; Funciones a implementar:
;   - indice_a_inventario
global EJERCICIO_1B_HECHO
EJERCICIO_1B_HECHO: db TRUE ; Cambiar por `TRUE` para correr los tests.

;########### ESTOS SON LOS OFFSETS Y TAMAÑO DE LOS STRUCTS
; Completar las definiciones (serán revisadas por ABI enforcer):
ITEM_NOMBRE EQU 0
ITEM_FUERZA EQU 20
ITEM_DURABILIDAD EQU 24
ITEM_SIZE EQU 28

;; La funcion debe verificar si una vista del inventario está correctamente 
;; ordenada de acuerdo a un criterio (comparador)

;; bool es_indice_ordenado(item_t** inventario, uint16_t* indice, uint16_t tamanio, comparador_t comparador);

;; Dónde:
;; - `inventario`: Un array de punteros a ítems que representa el inventario a
;;   procesar.
;; - `indice`: El arreglo de índices en el inventario que representa la vista.
;; - `tamanio`: El tamaño del inventario (y de la vista).
;; - `comparador`: La función de comparación que a utilizar para verificar el
;;   orden.
;; 
;; Tenga en consideración:
;; - `tamanio` es un valor de 16 bits. La parte alta del registro en dónde viene
;;   como parámetro podría tener basura.
;; - `comparador` es una dirección de memoria a la que se debe saltar (vía `jmp` o
;;   `call`) para comenzar la ejecución de la subrutina en cuestión.
;; - Los tamaños de los arrays `inventario` e `indice` son ambos `tamanio`.
;; - `false` es el valor `0` y `true` es todo valor distinto de `0`.
;; - Importa que los ítems estén ordenados según el comparador. No hay necesidad
;;   de verificar que el orden sea estable.

global es_indice_ordenado
; rdi ---> item_t** inventario
; rsi ---> uint16_t* indice
; dx  ---> uint16_t tamanio
; rcx ---> comparador_t comparador

es_indice_ordenado:
	; prologo
	push rbp
	mov rbp, rsp
	push r12
	push r13
	push r14
	push r15
	push rbx
	sub rsp, 8

; tienen que sobrevivir la llamada a funcion
	mov r12, rdi ; inventario
	mov r13, rsi ; indice
	movzx r14, dx ; tamanio
	mov r15, rcx ; comparador

	mov rbx, 0 ; -uint16_t- int i = 0
	sub r14, 1 ; tamanio = tamanio - 1
.loop:
	cmp rbx, r14  ; i < tamanio - 1 ??
	je .true

	mov r9, rbx ; bx 
	shl r9, 1 ; bx * 2
	movzx r8, word [r13 + r9] ; r8 = indice[i]
	mov rdi, [r12 + r8 * 8] ; actual = inventario[indice[i]]es

	inc rbx ; i++
	mov r9, rbx ; bx
	shl r9, 1 ; bx * 2
	movzx r8, word [r13 + r9] ; r8 = indice[i+1]
	mov rsi, [r12 + r8 * 8] ; sig = inventario[indice[i+1]]

	call r15 ; comparador(actual, sig), rax = 0 o 1
	;xor rax, 1 ; !comparador(actul, sig)

	cmp rax, 0 ; !comparador(actual, sig) == true?
	je .falso ; si es así, salto a falso

	; llego al siguiente ciclo con i = i+1 así que no vuelvo a incrementar
	jmp .loop

.falso:
	mov rax, 0
	jmp .fin

.true:
	mov rax, 1
.fin:
	;epilogo
	add rsp, 8
	pop rbx
	pop r15
	pop r14
	pop r13
	pop r12
	pop rbp
	ret


;; Dado un inventario y una vista, crear un nuevo inventario que mantenga el
;; orden descrito por la misma.

;; La memoria a solicitar para el nuevo inventario debe poder ser liberada
;; utilizando `free(ptr)`.

;; item_t** indice_a_inventario(item_t** inventario, uint16_t* indice, uint16_t tamanio);

;; Donde:
;; - `inventario` un array de punteros a ítems que representa el inventario a
;;   procesar.
;; - `indice` es el arreglo de índices en el inventario que representa la vista
;;   que vamos a usar para reorganizar el inventario.
;; - `tamanio` es el tamaño del inventario.
;; 
;; Tenga en consideración:
;; - Tanto los elementos de `inventario` como los del resultado son punteros a
;;   `ítems`. Se pide *copiar* estos punteros, **no se deben crear ni clonar
;;   ítems**

global indice_a_inventario
indice_a_inventario:
	; Te recomendamos llenar una tablita acá con cada parámetro y su
	; ubicación según la convención de llamada. Prestá atención a qué
	; valores son de 64 bits y qué valores son de 32 bits o 8 bits.
	;
	; rdi = item_t**  inventario
	; rsi = uint16_t* indice
	; dx = uint16_t  tamanio

	; prologo
	push rbp
	mov rbp, rsp
	push r12
	push r13
	push r14
	push r15
	push rbx
	sub rsp, 8

	mov r12, rdi ; inventario
	mov r13, rsi ; indice
	movzx r14, dx ; tamanio

	mov rdi, r14 ; tamanio
	shl rdi, 3
	call malloc ; rax <-- ptr a inicio de resultado

	xor rbx, rbx ; uint16_t i = 0

.loop:
	cmp rbx, r14 ; i < tamanio?
	je .fin

	movzx r15, word [r13 + rbx * 2] ; indice[i]
	mov r8, [r12 + r15 * 8] ; inventario[indice[i]] o sea inventario[r15]

	mov [rax + rbx * 8], r8 ; resultado[i] = inventario[indice[i]]
	inc rbx ; i++
	jmp .loop
.fin:
	; epilogo
	add rsp, 8
	pop rbx
	pop r15
	pop r14
	pop r13
	pop r12
	pop rbp
	ret