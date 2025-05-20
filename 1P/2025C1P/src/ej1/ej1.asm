extern malloc
extern sleep
extern wakeup
extern create_dir_entry

section .rodata
; Acá se pueden poner todas las máscaras y datos que necesiten para el ejercicio
sleep_name: DB "sleep", 0
wakeup_name: DB "wakeup", 0

section .text
; Marca un ejercicio como aún no completado (esto hace que no corran sus tests)
FALSE EQU 0
; Marca un ejercicio como hecho
TRUE  EQU 1

; Marca el ejercicio 1A como hecho (`true`) o pendiente (`false`).
;
; Funciones a implementar:
;   - init_fantastruco_dir
global EJERCICIO_1A_HECHO
EJERCICIO_1A_HECHO: db TRUE ; Cambiar por `TRUE` para correr los tests.

; Marca el ejercicio 1B como hecho (`true`) o pendiente (`false`).
;
; Funciones a implementar:
;   - summon_fantastruco
global EJERCICIO_1B_HECHO
EJERCICIO_1B_HECHO: db TRUE ; Cambiar por `TRUE` para correr los tests.

;########### ESTOS SON LOS OFFSETS Y TAMAÑO DE LOS STRUCTS
; Completar las definiciones (serán revisadas por ABI enforcer):
DIRENTRY_NAME_OFFSET EQU 0
DIRENTRY_PTR_OFFSET EQU 16
DIRENTRY_SIZE EQU 24

FANTASTRUCO_DIR_OFFSET EQU 0
FANTASTRUCO_ENTRIES_OFFSET EQU 8
FANTASTRUCO_ARCHETYPE_OFFSET EQU 16
FANTASTRUCO_FACEUP_OFFSET EQU 24
FANTASTRUCO_SIZE EQU 32

; void init_fantastruco_dir(fantastruco_t* card);
global init_fantastruco_dir
init_fantastruco_dir:
	; rdi = fantastruco_t*     card

	; prologo
	push rbp
	mov rbp, rsp
	push r12
	push r13
	push r14
	push r15
	push rbx
	sub rsp, 8 ; pila alineada

	mov r12, rdi ; r12 = card

	mov rdi, 16
	call malloc
	mov r13, rax ; r13 = dir

	mov rdi, sleep_name
	mov rsi, sleep
	call create_dir_entry
	mov r14, rax ; r14 = create_dir_entry("sleep", sleep)

	mov rdi, wakeup_name
	mov rsi, wakeup
	call create_dir_entry
	mov r15, rax ; r15 = create_dir_entry("wakeup", wakeup)

	mov [r13], r14 
	mov [r13 + 8], r15

	mov [r12 + FANTASTRUCO_DIR_OFFSET], r13
	mov ax, 2
	mov [r12 + FANTASTRUCO_ENTRIES_OFFSET], ax
	
	; epilogo
	add rsp, 8
	pop rbx
	pop r15
	pop r14
	pop r13
	pop r12
	pop rbp
	ret ;No te olvides el ret!

; fantastruco_t* summon_fantastruco();
global summon_fantastruco
summon_fantastruco:
	push rbp
	mov rbp, rsp
	push r12
	sub rsp, 8

	mov rdi, FANTASTRUCO_SIZE
	call malloc
	mov r12, rax ; r12 = card

	mov rdi, r12
	call init_fantastruco_dir

	mov r8, 0
	mov r9, 1
	mov [r12 + FANTASTRUCO_ARCHETYPE_OFFSET], r8
	mov [r12 + FANTASTRUCO_FACEUP_OFFSET], r9

	mov rax, r12 ; card	

	add rsp, 8
	pop r12
	pop rbp
	ret ;No te olvides el ret!
