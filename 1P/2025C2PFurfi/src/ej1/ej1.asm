extern malloc
extern strcpy

;########### SECCION DE DATOS
section .data

;########### SECCION DE TEXTO (PROGRAMA)
section .text


; Completar las definiciones (serán revisadas por ABI enforcer):
TUIT_MENSAJE_OFFSET EQU 0
TUIT_FAVORITOS_OFFSET EQU 140
TUIT_RETUITS_OFFSET EQU 142
TUIT_ID_AUTOR_OFFSET EQU 144
TUIT_SIZE EQU 148

PUBLICACION_NEXT_OFFSET EQU 0
PUBLICACION_VALUE_OFFSET EQU 8
PUBLICACION_SIZE EQU 16

FEED_FIRST_OFFSET EQU 0 
FEED_SIZE EQU 8

USUARIO_FEED_OFFSET EQU 0
USUARIO_SEGUIDORES_OFFSET EQU 8
USUARIO_CANT_SEGUIDORES_OFFSET EQU 16 
USUARIO_SEGUIDOS_OFFSET EQU 24
USUARIO_CANT_SEGUIDOS_OFFSET EQU 32 
USUARIO_BLOQUEADOS_OFFSET EQU 40
USUARIO_CANT_BLOQUEADOS_OFFSET EQU 48 
USUARIO_ID_OFFSET EQU 52
USUARIO_SIZE EQU 56

global agregar_al_feed
agregar_al_feed:
; rdi -> tuit_t* tuit
; rsi -> feed_t* feed

    push rbp
    mov rbp, rsp
    push r12
    push r13

    mov r12, rdi ; tuit
    mov r13, rsi ; feed

    mov rdi, PUBLICACION_SIZE
    call malloc ; rax <- publicacion

    mov rdx, [r13 + FEED_FIRST_OFFSET]
    mov [rax + PUBLICACION_NEXT_OFFSET], rdx ; publicacion->next = feed->first
    mov qword [rax + PUBLICACION_VALUE_OFFSET], r12 ; publicacion->value = tuit
    mov qword [r13 + FEED_FIRST_OFFSET], rax ; feed->first = publicacion

    pop r13
    pop r12
    pop rbp
    ret

; tuit_t *publicar(char *mensaje, usuario_t *usuario);
global publicar
publicar:
; rdi -> char *mensaje
; rsi -> usuario_t *user

    push rbp
    mov rbp, rsp
    push r12
    push r13
    push r14
    push r15
    push rbx
    sub rsp, 8

    mov r12, rdi ; mensaje
    mov r13, rsi ; user

    ; parte 1

    mov rdi, TUIT_SIZE
    call malloc ; rax <- res
    mov rbx, rax ; res

    lea rdi, [rbx + TUIT_MENSAJE_OFFSET] ; puntero dst
    mov rsi, r12 ; mensaje
    call strcpy

    mov word [rbx + TUIT_FAVORITOS_OFFSET], 0
    mov word [rbx + TUIT_RETUITS_OFFSET], 0
    mov r11d, dword [r13 + USUARIO_ID_OFFSET]
    mov dword [rbx + TUIT_ID_AUTOR_OFFSET], r11d

    ; parte 2
    mov rdi, rbx ; res
    mov rsi, qword [r13 + USUARIO_FEED_OFFSET]
    call agregar_al_feed

    ; parte 3
    mov r14, qword [r13 + USUARIO_SEGUIDORES_OFFSET] ; followers = user->seguidores
    mov r15, 0 ; i = 0
.loop:
    cmp r15d, dword [r13 + USUARIO_CANT_SEGUIDORES_OFFSET] ; i < user->cantSeguidores
    
    mov r8, [r14 + 8 * r15] ; followers[i]

    mov rdi, rbx ; res
    mov rsi, qword [r8 + USUARIO_FEED_OFFSET]
    call agregar_al_feed

    inc r15 ; i++ 

.fin:
    mov rax, rbx ; return
    add rsp, 8
    pop rbx
    pop r15
    pop r14
    pop r13
    pop r12
    pop rbp
    ret

