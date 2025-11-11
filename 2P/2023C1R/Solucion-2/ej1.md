## Ejercicio 1: a) Se pide implementar la syscall `exit()`, que desactiva la tarea que la llamó, y pone a ejecutar la siguiente.

Para eso, voy a agregar las estructuras necesarias al kernel para poder definir la nueva interrupción:

En isr.h:

```c
...
void _isr80();
```

En idt.c, dentro de la función idt_init():

```c
void idt_init() {
    ...
    IDT_ENTRY3(80); // porque queremos que sea llamada por tareas de usuario
```

Finalmente, hay que escribir el código de la interrupción:

En isr.asm:

```nasm
...
extern exit
...
global _isr80
_isr80:
    pushad
    ; Deshabilitar la tarea:
    push [current_task]
    call sched_disable_task
    add esp, 4

    call sched_next_task ; La próxima tarea a ejecutar
    
    cmp ax, 0
    je .fin

    mov word [sched_task_selector], ax

    jmp far [sched_task_offset]    
 
    fin:
    popad
    iret
```

Con esto, ya se tiene toda la funcionalidad pedida.

b)
Dentro de la rutina de atención a la interrupción, luego de obtener el selector de la próxima tarea a ejecutar, deberíamos acceder a la TSS de la misma y escribirle el EAX de la tarea que se estaba ejecutando previamente.

```nasm
...
extern escribir_EAX
...
_isr80:
    pushad
    push [current_task]
    call sched_disable_task
    add esp, 4

    call sched_next_task ; La próxima tarea a ejecutar
    
    cmp ax, 0
    jmp .fin

    push [current_task] ; id de la tarea que se está ejecutando
    push eax ; selector de la próxima tarea a ejecutar

    call escribir_EAX
    
    pop eax
    add esp, 4

    mov word [sched_task_selector], ax

    jmp far [sched_task_offset]

    .fin:
    popad
    iret
```

Ahora, hay que definir la función `void escribir_EAX(int8_t task_id, uint8_t task_sel)a`

```c
void escribir_EAX(int8_t task_id, uint16_t task_sel) {
    tss_t* tss = obtenerTSS(task_sel);
    uint32_t* pila = tss.esp;
    pila[7] = task_id; 
}

tss_t* obtenerTSS(uint16_t selector){
    uint16_t idx = selector >> 3;
    return gdt[idx].base; // en realidad acá es un abuso de anotación, ya que se guarda en partes en la GDT entry
}
```

c)
```nasm
global hubo_exit
hubo_exit: dd 0

global _isr32
_isr32:
    _isr32:
    pushad
    ; 1. Le decimos al PIC que vamos a atender la interrupción
    call pic_finish1
    call next_clock

    ; 2. Realizamos el cambio de tareas en caso de ser necesario
    call sched_next_task

    cmp ax, 0
    je .fin

    str bx
    cmp ax, bx
    je .fin

    cmp [hubo_exit], 0
    je .ejecucion_normal

    push [current_task] ; id de la tarea en ejecución
    push eax

    call escribir_EAX

    add esp, 8

    mov [hubo_exit], 0

.ejecucion_normal:
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

Ahora, hay que modificar la rutina de atención a la syscall exit, para que prenda el bit hubo_exit.

```nasm
global _isr80
_isr80:
    pushad
    ; Deshabilitar la tarea:
    push [current_task]
    call sched_disable_task
    add esp, 4

    call sched_next_task ; La próxima tarea a ejecutar
    
    cmp ax, 0
    je .fin

    mov [hubo_exit], 1
    
    mov word [sched_task_selector], ax

    jmp far [sched_task_offset]    
 
    fin:
    popad
    iret
```

