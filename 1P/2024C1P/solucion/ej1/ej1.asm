section .text

global inicializar_OT_asm
global calcular_z_asm
global ordenar_display_list_asm

extern malloc
extern free
extern calloc

NODO_DISPLAY_LIST_PRIMITIVA EQU 0
NODO_DISPLAY_LIST_X EQU 8
NODO_DISPLAY_LIST_Y EQU 9
NODO_DISPLAY_LIST_Z EQU 10
NODO_DISPLAY_LIST_SIGUIENTE EQU 16
NODO_DISPLAY_LIST_SIZE EQU 24

NODO_OT_T_DISPLAY_ELEMENT EQU 0
NODO_OT_T_SIGUIENTE EQU 8
NODO_OT_T_SIZE EQU 16

ORDERING_TABLE_T_TABLE_SIZE EQU 0
ORDERING_TABLE_T_TABLE EQU 8
ORDERING_TABLE_T_SIZE EQU 16

;########### SECCION DE TEXTO (PROGRAMA)

; ordering_table_t* inicializar_OT(uint8_t table_size);
inicializar_OT_asm:
    ; dil -> table_size
    
    ; epilogo
    push rbp
    mov rbp, rsp
    push r12
    push r13

    mov r12b, dil ; table_size

    mov rdi, ORDERING_TABLE_T_SIZE
    call malloc
    mov r13, rax ; ot

    mov byte [r13 + ORDERING_TABLE_T_TABLE_SIZE], r12b ; ot->table_size = table_size

    cmp r12b, 0 ; if (table_size == 0)
    je .setNull

    ; else
    movzx rdi, r12b ; table_size
    mov rsi, NODO_OT_T_SIZE
    call calloc
    mov [r13 + ORDERING_TABLE_T_TABLE], rax ; ot->table = calloc(...)
    jmp .fin

    .setNull:
    mov qword [r13 + ORDERING_TABLE_T_TABLE], 0 ; ot->table = NULL

    .fin:
    mov rax, r13 ; ot
    ; epilogo
    pop r13
    pop r12
    pop rbp
    ret

; void* calcular_z(nodo_display_list_t* display_list) ;
calcular_z_asm:
    ; rdi -> nodo
    ; sil -> z_size

    push rbp
    mov rbp, rsp
    push r12
    push r13

    mov r12, rdi ; nodo
    mov r13b, sil ; size

    .while:
    cmp r12, 0
    je .fin

    movzx rdi, byte [r12 + NODO_DISPLAY_LIST_X] ; act->x
    movzx rsi, byte [r12 + NODO_DISPLAY_LIST_Y] ; act->y
    movzx rdx, r13b ; z_size
    call [r12 + NODO_DISPLAY_LIST_PRIMITIVA]

    mov [r12 + NODO_DISPLAY_LIST_Z], rax

    mov r12, [r12 + NODO_DISPLAY_LIST_SIGUIENTE]

    .fin:
    pop r13
    pop r12
    pop rbp
    ret

; void* ordenar_display_list(ordering_table_t* ot, nodo_display_list_t* display_list) ;
ordenar_display_list_asm:
    ; rdi -> ordering_table_t* ot
    ; rsi -> nodo_display_list_t* display_list

    push rbp
    mov rbp, rsp
    push r12
    push r13
    push r14
    push r15
;    sub rsp, 8

    mov r12, rdi ; ot
    mov r13, rsi ; display_list

    cmp byte [r12 + ORDERING_TABLE_T_TABLE_SIZE], 0
    je .fin

    mov rdi, r13 ; display_list
    mov rsi, byte [r12 + ORDERING_TABLE_T_TABLE_SIZE], 0 ; ot->table_size
    call calcular_z_asm

    .while:
    cmp r13, 0 ; display_list != NULL
    je .fin

    movzx r14, byte [r13 + NODO_DISPLAY_LIST_Z] ; display_list->z
    mov r8, [r12 + ORDERING_TABLE_T_TABLE] ; ot->table
    mov r15, [r8 + r14 * 8] ; r15: nodo = ot->table[display_list->z]

    mov rdi, NODO_OT_T_SIZE ; sizeof(nodo_ot_t)
    call malloc

    mov [rax + NODO_OT_T_SIGUIENTE], 0 ; nodo_new->siguiente = NULL
    mov [rax + NODO_OT_T_DISPLAY_ELEMENT], r13 ; nodo_new->display_element = display_list

    cmp r15, 0 ; if (nodo == NULL)
    je .casoNull

    ; else
    .while2:
    cmp [r15 + NODO_OT_T_SIGUIENTE], 0 ; nodo->siguiente != NULL
    je .actualizarNodo

    .casoNull:
    mov r8, [r12 + ORDERING_TABLE_T_TABLE] ; ot->table
    mov [r8 + r14 * 8], rax ; ot->table[display_list->z] = nodo_new
    mov r13, [r13 + NODO_DISPLAY_LIST_SIGUIENTE] ; nodo = nodo->siguiente
    jmp .while

    .actualizarNodo:
    mov r13, [r13 + NODO_DISPLAY_LIST_SIGUIENTE] ; nodo = nodo->siguiente
    jmp .while

    .fin:
    pop r15
;    add rsp, 8
    pop r14
    pop r13
    pop r12
    pop rbp
    ret