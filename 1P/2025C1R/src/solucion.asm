; Definiciones comunes
TRUE  EQU 1
FALSE EQU 0

; Identificador del jugador rojo
JUGADOR_ROJO EQU 1
; Identificador del jugador azul
JUGADOR_AZUL EQU 2

; Ancho y alto del tablero de juego
tablero.ANCHO EQU 10
tablero.ALTO  EQU 5

; Marca un OFFSET o SIZE como no completado
; Esto no lo chequea el ABI enforcer, sirve para saber a simple vista qué cosas
; quedaron sin completar :)
NO_COMPLETADO EQU -1

extern strcmp

;########### ESTOS SON LOS OFFSETS Y TAMAÑO DE LOS STRUCTS
; Completar las definiciones (serán revisadas por ABI enforcer):
carta.en_juego EQU 0
carta.nombre   EQU 1
carta.vida     EQU 14
carta.jugador  EQU 16
carta.SIZE     EQU 18

tablero.mano_jugador_rojo EQU 0
tablero.mano_jugador_azul EQU 8
tablero.campo             EQU 16
tablero.SIZE              EQU 416

accion.invocar   EQU 0
accion.destino   EQU 8
accion.siguiente EQU 16
accion.SIZE      EQU 24

; Variables globales de sólo lectura
section .rodata

; Marca el ejercicio 1 como hecho (`true`) o pendiente (`false`).
;
; Funciones a implementar:
;   - hay_accion_que_toque
global EJERCICIO_1_HECHO
EJERCICIO_1_HECHO: db TRUE

; Marca el ejercicio 2 como hecho (`true`) o pendiente (`false`).
;
; Funciones a implementar:
;   - invocar_acciones
global EJERCICIO_2_HECHO
EJERCICIO_2_HECHO: db TRUE

; Marca el ejercicio 3 como hecho (`true`) o pendiente (`false`).
;
; Funciones a implementar:
;   - contar_cartas
global EJERCICIO_3_HECHO
EJERCICIO_3_HECHO: db TRUE

section .text

; Dada una secuencia de acciones determinar si hay alguna cuya carta tenga un
; nombre idéntico (mismos contenidos, no mismo puntero) al pasado por
; parámetro.
;
; El resultado es un valor booleano, la representación de los booleanos de C es
; la siguiente:
;   - El valor `0` es `false`
;   - Cualquier otro valor es `true`
;
; ```c
; bool hay_accion_que_toque(accion_t* accion, char* nombre);
; ```
global hay_accion_que_toque
hay_accion_que_toque:
	; Te recomendamos llenar una tablita acá con cada parámetro y su
	; ubicación según la convención de llamada. Prestá atención a qué
	; valores son de 64 bits y qué valores son de 32 bits o 8 bits.
	;
	; rdi = accion_t*  accion
	; rsi = char*      nombre
	push rbp
	mov rbp, rsp
	push r12
	push r13
	push r14
	push r15
	push rbx
	sub rsp, 8

	mov r12, rdi ; accion
	mov r13, rsi ; nombre

	cmp r12, 0
	je .false

	mov r14, r12 ; actual = accion

.loop:
	cmp r14, 0
	je .false

	mov rdi, [r14 + accion.destino] ; carta_t*
	add rdi, carta.nombre ; puntero al string
	mov rsi, r13 ; nombre
	call strcmp
	cmp rax, 0
	je .true

	mov r14, [r14 + accion.siguiente]
	jmp .loop

.true:
	mov rax, 1
	jmp .fin

.false:
	mov rax, 0
	jmp .fin

.fin:
	add rsp, 8
	pop rbx
	pop r15
	pop r14
	pop r13
	pop r12
	pop rbp
	ret

; Invoca las acciones que fueron encoladas en la secuencia proporcionada en el
; primer parámetro.
;
; A la hora de procesar una acción esta sólo se invoca si la carta destino
; sigue en juego.
;
; Luego de invocar una acción, si la carta destino tiene cero puntos de vida,
; se debe marcar ésta como fuera de juego.
;
; Las funciones que implementan acciones de juego tienen la siguiente firma:
; ```c
; void mi_accion(tablero_t* tablero, carta_t* carta);
; ```
; - El tablero a utilizar es el pasado como parámetro
; - La carta a utilizar es la carta destino de la acción (`accion->destino`)
;
; Las acciones se deben invocar en el orden natural de la secuencia (primero la
; primera acción, segundo la segunda acción, etc). Las acciones asumen este
; orden de ejecución.
;
; ```c
; void invocar_acciones(accion_t* accion, tablero_t* tablero);
; ```
global invocar_acciones
invocar_acciones:
	; Te recomendamos llenar una tablita acá con cada parámetro y su
	; ubicación según la convención de llamada. Prestá atención a qué
	; valores son de 64 bits y qué valores son de 32 bits o 8 bits.
	;
	; rdi = accion_t*  accion
	; rsi = tablero_t* tablero
	push rbp
	mov rbp, rsp
	push r12
	push r13
	push r14
	push r15
	push rbx
	sub rsp, 8

	mov r12, rdi ; accion
	mov r13, rsi ; tablero

	cmp r12, 0
	je .fin

	mov r14, r12 ; actual

.loop:
	cmp r14, 0
	je .fin

	mov r15, r14 ; actual
	mov r15, [r15 + accion.destino] ; actual->destino
	movzx rbx, byte [r15 + carta.en_juego] ; actual->destino->en_juego
	cmp rbx, 0
	je .nextIteration

	mov rdi, r13 ; tablero
	mov rsi, r15 ; actual->destino
	call [r14 + accion.invocar]; actual->invocar(tablero, actual->destino)

	movzx r8, word [r15 + carta.vida] ; actual->destino->vida
	cmp r8, 0
	jne .nextIteration

	mov byte [r15 + carta.en_juego], 0 ; actual->destino->en_juego = false

.nextIteration:
	mov r14, [r14 + accion.siguiente]
	jmp .loop

.fin:
	add rsp, 8
	pop rbx
	pop r15
	pop r14
	pop r13
	pop r12
	pop rbp
	ret

; Cuenta la cantidad de cartas rojas y azules en el tablero.
;
; Dado un tablero revisa el campo de juego y cuenta la cantidad de cartas
; correspondientes al jugador rojo y al jugador azul. Este conteo incluye tanto
; a las cartas en juego cómo a las fuera de juego (siempre que estén visibles
; en el campo).
;
; Se debe considerar el caso de que el campo contenga cartas que no pertenecen
; a ninguno de los dos jugadores.
;
; Las posiciones libres del campo tienen punteros nulos en lugar de apuntar a
; una carta.
;
; El resultado debe ser escrito en las posiciones de memoria proporcionadas
; como parámetro.
;
; ```c
; void contar_cartas(tablero_t* tablero, uint32_t* cant_rojas, uint32_t* cant_azules);
; ```
global contar_cartas
contar_cartas:
	; Te recomendamos llenar una tablita acá con cada parámetro y su
	; ubicación según la convención de llamada. Prestá atención a qué
	; valores son de 64 bits y qué valores son de 32 bits o 8 bits.
	;
	; rdi = tablero_t* tablero
	; rsi = uint32_t*  cant_rojas
	; rdx = uint32_t*  cant_azules
	push rbp
	mov rbp, rsp
	push r12
	push r13
	push r14
	push r15
	push rbx
	sub rsp, 8

	mov r12, rdi ; tablero
	mov r13, rsi ; cant_rojas
	mov r14, rdx ; cant_azules

	mov dword [r13], 0 ; cant_rojas = 0
	mov dword [r14], 0 ; cant_azules = 0

	mov r8, 0 ; indice = 0
	lea r12, [r12 + tablero.campo] ; tablero->campo
.loop:
	cmp r8, tablero.ANCHO*tablero.ALTO
	je .fin

	mov r10, [r12 + 8*r8] ; tablero->campo[indice]

	cmp r10, 0
	je .nextIteration

.jugador_rojo:
	cmp byte [r10 + carta.jugador], JUGADOR_ROJO
	jne .jugador_azul

	inc dword [r13] ; (*cant_rojas)++
	jmp .nextIteration

.jugador_azul:
	cmp byte [r10 + carta.jugador], JUGADOR_AZUL
	jne .nextIteration

	inc dword [r14] ; (*cant_azules)++

.nextIteration
	inc r8 ; indice++
	jmp .loop

.fin:
	add rsp, 8
	pop rbx
	pop r15
	pop r14
	pop r13
	pop r12
	pop rbp
	ret
