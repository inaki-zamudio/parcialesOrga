1) Para agregar la dos syscall nueva vamos a definir una interrupcion nueva en la idt modificando `idt_init()`.

Como las syscalls suelen definirse a partir del numero de interrupcion 80 va a ser la numero 80.

Como la interrupcion tiene que poder ser llamada desde el codigo de las tareas nivel 3 vamos a definirla como `IDT_ENTRY3`

Entonces en idt_init() agregamos:

```c
IDT_ENTRY3(80);
```
Tambien en `isr.h` hay que agregar:

```h
void _isr80();
```

La syscall va a recibir el valor a ser modificado en el mismo registro edx. 

Voy a agregar dos variables globales para el kernel dentro de `sched.c` una se va a llamar `hay_que_modificar_edx` que va a ser un booleano que indica si hay que modificar el edx de la tarea proxima a ejecutar. Y otra que se llama `valor_nuevo_edx` que va a contener efectivamente el valor a poner en el edx de la tarea que devolvemos. 

Para eso en `sched.c` agrego: 

```c
static paddr_t next_free_kernel_page = 0x100000;
static paddr_t next_free_user_page = 0x400000;

uint8_t hay_que_modificar_edx = 0;
uint32_t valor_nuevo_edx = 0;
```

2) El codigo de la interrupcion va a ser: 

```asm
global _isr80
_isr80:
    pushad
    mov [hay_que_modificar_edx], 1
    mov [valor_nuevo_edx], edx
    popad 
    iret
```
3) El codigo nuevo de la rutina de atencion del reloj va a ser: 

```asm
;; Rutina de atención del RELOJ
;; -------------------------------------------------------------------------- ;;
global _isr32

_isr32:
    pushad
    ; 1. Le decimos al PIC que vamos a atender la interrupción
    call pic_finish1
    call next_clock
    ; 2. Realizamos el cambio de tareas en caso de ser necesario
    call sched_next_task
    cmp [hay_que_modificar_edx], 0
    je .rutina_normal
    ; Si estamos aca es porque hay que modificar el edx de la proxima tarea a ejecutarse
    push ax 
    push DWORD [valor_nuevo_edx]
    call modificar_edx
    add esp, 4
    pop ax ; Restauro el selector
    mov [hay_que_modificar_edx], 0 ; Seteo la flag en 0 para que en el proximo clock no se vuelva a pisar
    mov [valor_nuevo_edx], 0
    .rutina_normal:
    cmp ax, 0
    je .fin

    str bx
    cmp ax, bx
    je .fin

    mov word [sched_task_selector], ax
    jmp far [sched_task_offset] 

    .fin:
    ; 3. Actualizamos las estructuras compartidas ante el tick del reloj
    call tasks_tick
    ; 4. Actualizamos la "interfaz" del sistema en pantalla
    call tasks_screen_update
    popad
    iret
```

Y agrego la funcion `modificar_edx` en `tss.c` importando la gdt: 

```c
void modificar_edx(uint32_t valor_a_poner_en_edx,uint16_t segsel) {
    uint16_t idx = segsel >> 3;
    tss_t* tss_task = (tss_t*)((gdt[idx].base_15_0) | (gdt[idx].base_23_16 << 16) | (gdt[idx].base_31_24 << 24));
    uint32_t* pila = tss_task->esp;
    pila[5] = valor_a_poner_en_edx;
}
```