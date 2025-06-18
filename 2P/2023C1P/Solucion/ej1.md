A) Suponiendo que no hay que hablar de la creacion de las tareas para definir el servicio:

Para agregar agregar la syscall vamos a definir una interrupcion nueva en `idt_init()`

Como las syscalls suelen definirse a partir del numero de interrupcion 80 la vamos a definir como la interrupcion numero 80.

Como queremos que la interrupcion pueda ser llamada desde codigo nivel 3 de las tareas la vamos a definir como una `IDT_ENTRY3`

Entonces en idt_init() agregamos:

```c
IDT_ENTRY3(80);
```

Por ultimo en `isr.h` hay que agregar: 

```h
void _isr80();
```

B) Desde la syscall lo que vamos a hacer va a ser pausar a la tarea, establecer a la sexta tarea como runnable por si se interrumpe en el medio de la ejecucion y saltar a la sexta tarea. La sexta tarea se va a encargar de hacer el calculo y modificar una variable global indicando que hay que despausar a la tarea que la llamo porque ya terminamos de procesar la sexta tarea. 


La sexta tarea va a recibir tambien en ecx el id de la tarea que la llamo para que cuando la sexta tarea termine despause a la tarea que la llamo y al final se pause a ella misma

Definimos la syscall en `isr.asm`:

```asm
global _isr80
_isr80:
    pushad
    push eax
    push DWORD [current_task]
    call sched_disable_task
    add esp, 4
    push [SELECTOR_SEXTA_TAREA]
    call modificar_eax
    push DWORD [current_task]
    call modificar_ecx
    add esp, 12
    push [ID_SEXTA_TAREA]
    call sched_enable_task
    add esp, 2
    mov ax, [SELECTOR_SEXTA_TAREA] ; Habria que definir como una constante global este selector
    mov edx, [ID_SEXTA_TAREA] ; Habria que definir como una constante global este id
    mov [current_task], edx
    mov word [sched_task_selector], ax
    jmp far [sched_task_offset] 
    ; Pausamos de nuevo a la sexta tarea para que no se vuelva a correr
    push [ID_SEXTA_TAREA]
    call sched_disable_task
    popad
    iret
```

Definimos en `tss.c` la funcion `modificar_eax` y `modificar_ecx` importando la gdt

```c
void modificar_eax(uint16_t segsel, uint32_t eax_a_modificar){
  uint16_t idx = segsel >> 3;
  tss_t* tss_task = (tss_t*)((gdt[idx].base_15_0) | (gdt[idx].base_23_16 << 16) | (gdt[idx].base_31_24 << 24));
  uint32_t* pila = tss_task->esp;
  pila[7] = eax_a_modificar; 
}

void modificar_ecx(uint32_t id_tarea_llamadora, uint16_t segsel){
  uint16_t idx = segsel >> 3;
  tss_t* tss_task = (tss_t*)((gdt[idx].base_15_0) | (gdt[idx].base_23_16 << 16) | (gdt[idx].base_31_24 << 24));
  uint32_t* pila = tss_task->esp;
  pila[6] = id_tarea_llamadora; 
}
```
C) Como la sexta tarea tiene nivel de privilegio 0 va a poder llamar a las funciones de `mmu.c` para despausar a la tarea

El codigo de la tarea va a ser: 

```asm
add eax, 2
push ecx 
call sched_enable_task
add esp, 4
```
