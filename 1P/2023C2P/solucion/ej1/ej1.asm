section .text

global contar_pagos_aprobados_asm
global contar_pagos_rechazados_asm

global split_pagos_usuario_asm

extern malloc
extern free
extern strcmp

PAGO_T_MONTO EQU 0
PAGO_T_APROBADO EQU 1
PAGO_T_PAGADOR EQU 8
PAGO_T_COBRADOR EQU 16
PAGO_T_SIZE EQU 24

PAGOSPLITTED_T_CANT_APROBADOS EQU 0
PAGOSPLITTED_T_CANT_RECHAZADOS EQU 1
PAGOSPLITTED_T_APROBADOS EQU 8
PAGOSPLITTED_T_RECHAZADOS EQU 16
PAGOSPLITTED_T_SIZE EQU 24

LISTELEM_T_DATA EQU 0
LISTELEM_T_NEXT EQU 8
LISTELEM_T_PREV EQU 16
LISTELEM_T_SIZE EQU 24

LIST_T_FIRST EQU 0
LIST_T_LAST EQU 8

;########### SECCION DE TEXTO (PROGRAMA)

; uint8_t contar_pagos_aprobados_asm(list_t* pList, char* usuario);
contar_pagos_aprobados_asm:
    ; rdi -> pList
    ; rsi -> usuario

	; prologo
	push rbp
	mov rbp, rsp
	push r12
	push r13
	push r14
	push r15
	push rbx
	sub rsp, 8 ; pila alineada

    mov r12, rdi ; pList
    mov r13, rsi ; usuario

    mov bl, 0 ; aprobados

    cmp qword r12, 0
    je .fin

    mov r14, [r12 + LIST_T_FIRST] ; act

    .loop:
    cmp r14, 0 ; act != NULL
    je .fin

    mov r15, [r14 + LISTELEM_T_DATA] ; act->data
    mov rdi, [r15 + PAGO_T_COBRADOR] ; act->data->cobrador
    mov rsi, r13 ; usuario
    call strcmp

    cmp rax, 0
    jne .nextIteration

    cmp byte [r15 + PAGO_T_APROBADO], 1 ; act->data->aprobado
    jne .nextIteration

    inc bl

    .nextIteration:
    mov r14, [r14 + LISTELEM_T_NEXT]
    jmp .loop

    .fin:
    xor rax, rax
    mov al, bl
	; epilogo
	add rsp, 8 ; restauro pila
	pop rbx
	pop r15
	pop r14
	pop r13
	pop r12
	pop rbp
	ret

; uint8_t contar_pagos_rechazados_asm(list_t* pList, char* usuario);
contar_pagos_rechazados_asm:
; rdi -> pList
    ; rsi -> usuario

	; prologo
	push rbp
	mov rbp, rsp
	push r12
	push r13
	push r14
	push r15
	push rbx
	sub rsp, 8 ; pila alineada

    mov r12, rdi ; pList
    mov r13, rsi ; usuario

    mov rbx, 0 ; rechazados

    cmp qword r12, 0
    je .fin

    mov r14, [r12 + LIST_T_FIRST] ; act

    .loop:
    cmp r14, 0 ; act != NULL
    je .fin

    mov r15, [r14 + LISTELEM_T_DATA] ; act->data
    mov rdi, [r15 + PAGO_T_COBRADOR] ; act->data->cobrador
    mov rsi, r13 ; usuario
    call strcmp

    cmp rax, 0
    jne .nextIteration

    cmp byte [r15 + PAGO_T_APROBADO], 0 ; act->data->aprobado == 0
    jne .nextIteration

    inc rbx

    .nextIteration:
    mov r14, [r14 + LISTELEM_T_NEXT]
    jmp .loop

    .fin:
    mov rax, rbx
	; epilogo
	add rsp, 8 ; restauro pila
	pop rbx
	pop r15
	pop r14
	pop r13
	pop r12
	pop rbp
	ret


; pagoSplitted_t* split_pagos_usuario_asm(list_t* pList, char* usuario);
split_pagos_usuario_asm:
    ; rdi --> pList
    ; rsi --> usuario

; prologo
	push rbp
	mov rbp, rsp
	push r12
	push r13
	push r14
	push r15
	push rbx
	sub rsp, 8 ; pila alineada

    mov r12, rdi ; pList
    mov r13, rsi ; usuario

    mov rdi, PAGOSPLITTED_T_SIZE
    call malloc
    mov r14, rax ; res

    mov rdi, r12 ; pList
    mov rsi, r13 ; usuario
    call contar_pagos_aprobados_asm
    mov byte [r14 + PAGOSPLITTED_T_CANT_APROBADOS], al ; res->cant_aprobados = contar_pagos_aprobados(pList, usuario)

    mov rdi, r12
    mov rsi, r13
    call contar_pagos_rechazados_asm
    mov byte [r14 + PAGOSPLITTED_T_CANT_RECHAZADOS], al ; res->cant_rechazados = contar_pagos_rechazados(pList, usuario)

    movzx rdi, byte [r14 + PAGOSPLITTED_T_CANT_APROBADOS] ; res->cant_aprobados
    shl rdi, 3 ; 8*rdi
    call malloc
    mov [r14 + PAGOSPLITTED_T_APROBADOS], rax

    movzx rdi, byte [r14 + PAGOSPLITTED_T_CANT_RECHAZADOS] ; res->cant_rechazados
    shl rdi, 3 ; 8*rdi
    call malloc
    mov [r14 + PAGOSPLITTED_T_RECHAZADOS], rax

    cmp r12, 0
    je .fin

    mov r15, [r12 + LIST_T_FIRST] ; act
    mov r12, 0 ; ultapr
    mov rbx, 0 ; ultrech

    .loop:
    cmp r15, 0 ; act != NULL
    je .fin

    mov rdi, [r15 + LISTELEM_T_DATA]
    mov rdi, [rdi + PAGO_T_COBRADOR] ; act->data->cobrador
    mov rsi, r13 ; usuario
    call strcmp

    cmp rax, 0 ; strcmp(act->data->cobrador, usuario)
    jne .nextIteration

    mov r8, [r15 + LISTELEM_T_DATA] ; act->data
    movzx r8, byte [r8 + PAGO_T_APROBADO] ; act->data->aprobado
    cmp r8, 1
    je .agregarAprobado

    mov r9, [r14 + PAGOSPLITTED_T_RECHAZADOS] ; res->rechazados
    mov [r9 + rbx*8], r15
    inc rbx ; ultrech++
    jmp .nextIteration

    .agregarAprobado:
    mov r9, [r14 + PAGOSPLITTED_T_APROBADOS] ; res->aprobados
    mov [r9 + r12*8], r15
    inc r12 ; ultapr++

    .nextIteration:
    mov r15, [r15 + LISTELEM_T_NEXT]
    jmp .loop

    .fin:
    mov rax, r14
    add rsp, 8 ; restauro pila
	pop rbx
	pop r15
	pop r14
	pop r13
	pop r12
	pop rbp
	ret