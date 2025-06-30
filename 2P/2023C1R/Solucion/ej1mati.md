A) Para implementar la syscall lo que hay que hacer es definir una interrupcion nueva. Para eso vamos a definir una interrupcion nueva en `idt_init()`.

Como las syscalls suelen definirse a partir del numero de interrupcion 80 vamos a definirla como la numero 80.

Para que pueda ser llamada desde las tareas va a ser una `IDT_ENTRY3`. Entonces en la funcion `idt_init` agregamos: 

```c
// COMPLETAR: Syscalls
IDT_ENTRY3(80);
IDT_ENTRY3(88);
IDT_ENTRY3(98);
```

Tambien en isr.h hay que agregar:

```h
void _isr80();
```

El codigo de atencion de la interrupcion exit queda definido como: 

```asm
global _isr80
_isr80:
    pushad
    ; Deshabilitamos la tarea actual
    push DWORD [current_task]
    call sched_disable_task
    add esp, 4
    call sched_next_task
    cmp ax, 0
    je .fin

    str bx
    cmp ax, bx
    je .fin

    mov word [sched_task_selector], ax
    jmp far [sched_task_offset] 
    .fin: 
      popad
      iret
```

B) Para que exit tambien guarde el id de la tarea que la llamo en el eax de la proxima tarea a ejecutar voy a meterme en la pila de la tss de la proxima tarea a ejecutar y voy a modificar el valor de su eax. 

El codigo de atencion de la interrupcion queda: 

```asm
global _isr80
_isr80:
    pushad
    ; Deshabilitamos la tarea actual
    push DWORD [current_task]
    call sched_disable_task
    add esp, 4
    call sched_next_task
    ; sched_next_task nos devuelve el selector de la siguiente tarea a ejecutar. 
    push DWORD [current_task]
    push ax
    call guardar_id
    pop ax ; Restauramos el valor del selector de la tarea a saltar
    add esp, 4
    cmp ax, 0
    je .fin

    str bx
    cmp ax, bx
    je .fin

    mov word [sched_task_selector], ax
    jmp far [sched_task_offset] 
    .fin: 
      popad
      iret
```

Y en `tss.c` importamos la gdt y agregamos la funcion `guardar_id`:

```c
void guardar_id(uint16_t segsel, uint32_t id_tarea){
  uint16_t idx = segsel >> 3;
  tss_t* tss_task = (tss_t*)((gdt[idx].base_15_0) | (gdt[idx].base_23_16 << 16) | (gdt[idx].base_31_24 << 24));
  uint32_t* pila = tss_task->esp;
  pila[7] = id_tarea; 
}
```

C) Si ahora el que tiene que modificar el eax si se produjo un exit es la interrupcion de reloj voy a agregar dos variables globales dentro del archivo de las interrupciones: Una que se va a llamar `hubo_exit` y la otra `id_tarea_que_hizo_exit`. 

Asi que al principio de `isr.asm` agregamos: 

```asm
section .data
global hubo_exit    
global id_tarea_que_hizo_exit  

hubo_exit:   db 0  
id_tarea_que_hizo_exit: dd 0    
```

Y ahora en la interrupcion del exit lo que se va a hacer es setear estas variables globales.

```asm
global _isr80
_isr80:
    pushad
    ; Deshabilitamos la tarea actual
    push DWORD [current_task]
    call sched_disable_task
    add esp, 4
    ; Ahora hacemos que la variable global hubo_exit sea 1 y que tarea_que_hizo_exit sea igual a current_task
    mov [hubo_exit], 1
    mov ecx, [current_task]
    mov [tarea_que_hizo_exit], ecx
    call sched_next_task
    cmp ax, 0
    je .fin

    str bx
    cmp ax, bx
    je .fin

    mov word [sched_task_selector], ax
    jmp far [sched_task_offset] 
    .fin: 
      popad
      iret
```

Y ahora la rutina de atencion del reloj va a ser la encargada de actualizar el eax de la tarea si es que la tarea previa hizo un exit: 

```asm
global _isr32

_isr32:
    pushad
    ; 1. Le decimos al PIC que vamos a atender la interrupción
    call pic_finish1
    call next_clock
    ; Primero nos fijamos si hay que actualizarle el eax a la tarea saliente
    mov al, [hubo_exit]
    cmp al, 0
    je .rutina_normal
    ; Si estamos aca es porque hay que actualizar el eax de esta tarea 
    call selector_segmento_actual ; Consigo el selector de segmento actual
    push ax 
    push [id_tarea_que_hizo_exit]
    call actualizar_eax_tarea_saliente
    add esp, 5 
    ; Actualizamos las variables globales para que en el proximo clock no se toque ningun eax si no se hizo ningun exit
    mov [hubo_exit], 0 
    mov [id_tarea_que_hizo_exit], 0
    .rutina_normal:
    ; 2. Realizamos el cambio de tareas en caso de ser necesario
    call sched_next_task
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

En `sched.c` defino la funcion: 

```c
uint16_t selector_segmento_actual(){
    return sched_tasks[current_task].selector;
}
```

Y en `tss.c` defino la funcion: 

```c
void actualizar_eax_tarea_saliente(uint32_t id_tarea_hizo_exit, uint16_t segsel){
  uint16_t idx = segsel >> 3;
  tss_t* tss_task = (tss_t*)((gdt[idx].base_15_0) | (gdt[idx].base_23_16 << 16) | (gdt[idx].base_31_24 << 24));
  uint32_t* pila = tss_task->esp;
  pila[7] = id_tarea_hizo_exit;
}

```
