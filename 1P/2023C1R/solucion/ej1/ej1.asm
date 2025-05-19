global acumuladoPorCliente_asm
global en_blacklist_asm
global blacklistComercios_asm

extern calloc
extern malloc
extern strcmp

;########### SECCION DE TEXTO (PROGRAMA)
section .text

PAGO_T_MONTO EQU 0
PAGO_T_COMERCIO EQU 8
PAGO_T_CLIENTE EQU 16
PAGO_T_APROBADO EQU 17
PAGO_T_SIZE EQU 24

acumuladoPorCliente_asm:
	; dil -> cantidadDePagos
	; rsi -> arr_pagos

	; prologo
	push rbp
	mov rbp, rsp
	push r12
	push r13
	push r14
	push r15
	push rbx
	sub rsp, 8 ; pila alineada

	movzx r12, dil ; cantidadDePagos
	mov r13, rsi ; arr_pagos

	mov rdi, 10
	mov rsi, 4
	call calloc
	mov r14, rax ; res

	mov r15, 0 ; i 
	
	.loop:
	cmp r15, r12 ; i < cantidadDePagos
	je .fin

	mov r9, r15 ; i
	imul r9, PAGO_T_SIZE ; i*pago_t_size
	lea rbx, [r13 + r9] ; pago = arr_pagos[i]

	cmp byte [rbx + PAGO_T_APROBADO], 0
	je .nextIteration

	movzx r8d, byte [rbx + PAGO_T_CLIENTE] ; idx = (uint8_t) cliente
	movzx r9d, byte [rbx + PAGO_T_MONTO]   ; monto = (uint8_t) monto

	mov eax, [r14 + r8*4] ; res[idx]
	add eax, r9d
	mov [r14 + r8*4], eax ; res[idx] += monto

	.nextIteration:
	inc r15  ;i++
	jmp .loop

	.fin:
	mov rax, r14
	; epilogo
	add rsp, 8 ; restauro pila
	pop rbx
	pop r15
	pop r14
	pop r13
	pop r12
	pop rbp
	ret

en_blacklist_asm:
	; rdi -> comercio
	; rsi -> lista_comercios
	; dl -> n

	; prologo
	push rbp
	mov rbp, rsp
	push r12
	push r13
	push r14
	push r15
	push rbx
	sub rsp, 8 ; pila alineada

	mov r12, rdi ; comercio
	mov r13, rsi ; lista_comercios
	movzx r14, dl ; n

	mov r15, 0 ; i
	mov rbx, 0 ; resultado

	.loop:
	cmp r15, r14 ; i == n
	jge .fin

	mov rdi, [r13 + r15*8] ; lista_comercios[i]
	mov rsi, r12 ; comercio
	call strcmp

	cmp rax, 0 ; if (strcmp(lista_comercios[i], comercio) == 0)
	je .tru

	inc r15
	jmp .loop

	.tru:
	mov rbx, 1

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

blacklistComercios_asm:
	; dil -> cantidad_pagos
	; rsi -> arr_pagos
	; rdx -> arr_comercios
	; cl  -> size_comercios
	
	; prologo
	push rbp
	mov rbp, rsp
	push r12
	push r13
	push r14
	push r15
	push rbx
	; sub rsp, 8 ; pila alineada

	movzx r12, dil ; cantidad_pagos
	mov r13, rsi ; arr_pagos
	mov r14, rdx ; arr_comercios
	mov r15b, cl ; size_comercios

	mov rbx, 0 ; cant_pagos_blacklisteados = 0
	mov r8, 0 ; i

	.loop:
	cmp r8, r12 ; i == cantidad_pagos
	je .blacklistear ; paso a la parte 2

	push r8 ; ya q es volátil y quiero que sobreviva al call
	
	mov r9, r8 ; i
	imul r9, PAGO_T_SIZE ; i*sizeof(pago_t)
	lea rdi, [r13 + r9] ; pago = arr_pagos[i]

	mov rdi, [rdi + PAGO_T_COMERCIO] ; arr_pagos[i].comercio
	mov rsi, r14 ; arr_comercios
	mov dl, r15b ; size_comercios
	call en_blacklist_asm ; res = rax

	cmp rax, 0 ; if (en_blacklist(arr_pagos[i].comercio, arr_comercios, size_comercios))
	je .nextIteration

	inc rbx ; cant_pagos_blacklisteados++
	
	.nextIteration:
	pop r8 ; popeo al r8 de la pila
	inc r8 ; i++
	jmp .loop

	.blacklistear:
	mov rdi, 8
	imul rdi, rbx  ; sizeof(pago_t*)*cant_pagos_blacklisteados
	call malloc
	mov rbx, rax ; res

	mov r8, 0 ; r8 = i = 0
	mov r10, 0 ; r10 = idx = 0

	.loop2:
	cmp r8, r12 ; i == cant_pagos
	je .fin

	push r8 ; (i) ya q es volátil y quiero q sobreviva al call
	push r10 ; (idx)

	mov r9, r8 ; i
	imul r9, PAGO_T_SIZE ; i*sizeof(pago_t)
	lea rdi, [r13 + r9] ; pago = arr_pagos[i]

	mov rdi, [rdi + PAGO_T_COMERCIO] ; arr_pagos[i].comercio
	mov rsi, r14 ; arr_comercios
	mov dl, r15b ; size_comercios
	call en_blacklist_asm ; res = rax

	pop r10 ; recupero idx
	pop r8 ; lo necesito usar

	cmp rax, 0
	je .nextIteration2

	mov rax, r8
	imul rax, PAGO_T_SIZE
	lea r11, [r13 + rax] ; &arr[pagos[i]
	mov [rbx + r10 * 8], r11 ; res[idx] = &arr_pagos[i]

	inc r10 ; idx++ 
	inc r8 ; i++
	jmp .loop2

	.nextIteration2:
	inc r8 ; i++
	jmp .loop2

	.fin:
	mov rax, rbx
	; epilogo
	; add rsp, 8 ; restauro pila
	pop rbx
	pop r15
	pop r14
	pop r13
	pop r12
	pop rbp
	ret
