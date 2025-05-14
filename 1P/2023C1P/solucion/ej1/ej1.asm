extern malloc

global templosClasicos
global cuantosTemplosClasicos



;########### SECCION DE TEXTO (PROGRAMA)
section .text

TEMPLO_COLUM_LARGO EQU 0
TEMPLO_NOMBRE EQU 8
TEMPLO_COLUM_CORTO EQU 16
TEMPLO_SIZE EQU 24

templosClasicos:
    ; rdi --> templo* temploArr
    ; rsi --> size_t temploArr_len

    ; prologo
	push rbp
	mov rbp, rsp
	push r12
	push r13
	push r14
	push r15
	push rbx
	sub rsp, 8 ; pila alineada

    mov r12, rdi ; temploArr
    mov r13, rsi ; temploArr_len

    call cuantosTemplosClasicos ; rax = tam_res
    
    mov rdi, rax
    imul rdi, TEMPLO_SIZE
    call malloc ; rax = ptr a templo* (nuevoarr)
    
    mov rbx, 0 ; ebx = ult_indice
    mov r8, 0 ; r8 = i

    .loop:
    cmp r8, r13 ; i < temploArr_len ?
    je .fin

    mov r14, r8 ; i
    imul r14, 24 ; indice * tam_dato

    movzx r15, byte [r12 + r14 + TEMPLO_COLUM_LARGO] ; largo
    movzx r9, byte [r12 + r14 + TEMPLO_COLUM_CORTO] ; corto

    shl r9, 1 ; 2*corto
    inc r9 ; 2*corto + 1

    cmp r15, r9 ; largo == 2*corto+1
    jne .nextIteration

    mov r10, rbx
    imul r10, 24

    mov r11, [r12 + r14]
    mov [rax + r10], r11

    mov r11, [r12 + r14 + 8]
    mov [rax + r10 + 8], r11

    mov r11, [r12 + r14 + 16]
    mov [rax + r10 + 16], r11
    inc rbx

    .nextIteration:
    inc r8
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

cuantosTemplosClasicos:
    ; rdi --> temploArr
    ; rsi --> temploArr_len

	; prologo
	push rbp
	mov rbp, rsp
	push r12
	push r13
	push r14
	push r15
	push rbx
	sub rsp, 8 ; pila alineada

    mov r12, rdi ; temploArr
    mov r13, rsi ; temploArr_len

    mov rax, 0 ; res
    mov r9, 0 ; i

.loop:
    cmp r9, r13
    je .fin

    mov r14, r9 ; i
    imul r14, 24 ; indice * tam_dato
 
    movzx r15, byte [r12 + r14 + TEMPLO_COLUM_LARGO] ; largo
    movzx r8, byte [r12 + r14 + TEMPLO_COLUM_CORTO] ; corto

    shl r8, 1
    inc r8 ; r8b = 2*corto+1

    cmp r15, r8
    je .incRes

    inc r9
    jmp .loop

.incRes:
    inc rax
    inc r9
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

