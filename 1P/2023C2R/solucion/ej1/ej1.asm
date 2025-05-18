; /** defines bool y puntero **/
%define NULL 0
%define TRUE 1
%define FALSE 0

section .data

section .text

global string_proc_list_create_asm
global string_proc_node_create_asm
global string_proc_list_add_node_asm
global string_proc_list_concat_asm

; FUNCIONES auxiliares que pueden llegar a necesitar:
extern malloc
extern free
extern str_concat

STRING_PROC_NODE_NEXT EQU 0
STRING_PROC_NODE_PREVIOUS EQU 8
STRING_PROC_NODE_TYPE EQU 16
STRING_PROC_NODE_HASH EQU 24
STRING_PROC_NODE_SIZE EQU 32

STRING_PROC_LIST_FIRST EQU 0
STRING_PROC_LIST_LAST EQU 8
STRING_PROC_LIST_SIZE EQU 16

string_proc_list_create_asm:
    ; esta función no recibe argumentos
    
    ; prologo
	push rbp
	mov rbp, rsp
	push r12
	push r13
	push r14
	push r15
	push rbx
	sub rsp, 8 ; pila alineada

    mov rdi, STRING_PROC_LIST_SIZE ; 16
    call malloc
    mov r12, rax
    ; seteo los punteros a null
    mov qword [r12 + STRING_PROC_LIST_FIRST], 0
    mov qword [r12 + STRING_PROC_LIST_LAST], 0

	; epilogo
	add rsp, 8 ; restauro pila
	pop rbx
	pop r15
	pop r14
	pop r13
	pop r12
	pop rbp
	ret

string_proc_node_create_asm:
    ; dil --> type
    ; rsi --> hash
    push rbp
	mov rbp, rsp
	push r12
	push r13
	push r14
	push r15
	push rbx
	sub rsp, 8 ; pila alineada

    mov r12b, dil ; type
    mov r13, rsi ; hash

    mov rdi, STRING_PROC_NODE_SIZE
    call malloc
    mov r14, rax ; nodo

    mov qword [r14 + STRING_PROC_NODE_NEXT], 0 ; nodo->next = NULL
    mov qword [r14 + STRING_PROC_NODE_PREVIOUS], 0 ; nodo->previous = NULL
    mov byte [r14 + STRING_PROC_NODE_TYPE], r12b ; nodo->type = type
    mov qword [r14 + STRING_PROC_NODE_HASH], r13 ; nodo->hash = hash

    mov rax, r14 ; return nodo

	; epilogo
	add rsp, 8 ; restauro pila
	pop rbx
	pop r15
	pop r14
	pop r13
	pop r12
	pop rbp
	ret

string_proc_list_add_node_asm:
    ; rdi --> list
    ; sil --> type
    ; rdx --> hash

    push rbp
	mov rbp, rsp
	push r12
	push r13
	push r14
	push r15
	push rbx
	sub rsp, 8 ; pila alineada

    mov r12, rdi ; list
    mov r13b, sil ; type
    mov r14, rdx ; hash

    mov dil, r13b
    mov rsi, r14
    call string_proc_node_create_asm
    mov r15, rax ; nodo

    cmp qword [r12 + STRING_PROC_LIST_FIRST], 0 ; if (list->first == NULL)
    je .listaVacia

	mov r9, [r12 + STRING_PROC_LIST_LAST]
    mov qword [r15 + STRING_PROC_NODE_PREVIOUS], r9 ; nodo->previous = list->last
    mov r8, [r12 + STRING_PROC_LIST_LAST]
	mov qword [r8 + STRING_PROC_NODE_NEXT], r15 ; list->last->next = nodo

	mov qword [r12 + STRING_PROC_LIST_LAST], r15 ; list->last = nodo
	jmp .fin

    .listaVacia:
    mov qword [r12 + STRING_PROC_LIST_FIRST], r15 ; list->first = nodo
    mov qword [r12 + STRING_PROC_LIST_LAST], r15 ; list->last = nodo

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

string_proc_list_concat_asm:
