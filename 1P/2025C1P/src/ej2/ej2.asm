extern strcmp
global invocar_habilidad

; Completar las definiciones o borrarlas (en este ejercicio NO serán revisadas por el ABI enforcer)
DIRENTRY_NAME_OFFSET EQU 0
DIRENTRY_PTR_OFFSET EQU 16
DIRENTRY_SIZE EQU 24

FANTASTRUCO_DIR_OFFSET EQU 0
FANTASTRUCO_ENTRIES_OFFSET EQU 8
FANTASTRUCO_ARCHETYPE_OFFSET EQU 16
FANTASTRUCO_FACEUP_OFFSET EQU 24
FANTASTRUCO_SIZE EQU 32

section .rodata
; Acá se pueden poner todas las máscaras y datos que necesiten para el ejercicio

section .text

; void invocar_habilidad(void* carta, char* habilidad);
invocar_habilidad:
	; Te recomendamos llenar una tablita acá con cada parámetro y su
	; ubicación según la convención de llamada. Prestá atención a qué
	; valores son de 64 bits y qué valores son de 32 bits o 8 bits.
	;
	; rdi = void*    card ; Vale asumir que card siempre es al menos un card_t*
	; rsi = char*    habilidad

	; prologo
	push rbp
	mov rbp, rsp
	push r12
	push r13
	push r14
	push r15
	;sub rsp, 8

	mov r12, rdi ; carta
	mov r13, rsi ; habilidad

	.while:
	cmp r12, 0 ; while (carta != NULL)
	je .fin

	xor r14, r14 ; r14 = i

	.for:
	cmp r14w, word [r12 + FANTASTRUCO_ENTRIES_OFFSET] ; i < carta->__dir_entries
	je .whileNextIteration

	mov r15, qword [r12 + FANTASTRUCO_DIR_OFFSET] ; r15 = carta->__dir (ptr al inicio del array)
	mov r15, [r15 + r14 * 8] ; r15 = carta->__dir[i]
	
	mov rdi, r15 ; rdi = carta->__dir[i]->ability_name

	mov rsi, r13 ; rsi = habilidad

	call strcmp
	
	cmp rax, 0 ; if (strcmp(carta->__dir[i]->ability_name, habilidad) == 0)
	jne .forNextIteration

	mov rdi, r12
	call [r15 + DIRENTRY_PTR_OFFSET]
	jmp .fin

	.forNextIteration:
	inc r14w ; i++
	jmp .for

	.whileNextIteration:
	mov r12, [r12 + FANTASTRUCO_ARCHETYPE_OFFSET]
	jmp .while

	.fin:
	;add rsp, 8
	pop r15
	pop r14
	pop r13
	pop r12
	pop rbp
	ret ;No te olvides el ret!
