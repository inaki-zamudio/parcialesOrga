a)
> Características del servicio: 
> - 5 tareas nivel 3 (1)
> - 1 tarea nivel 0 (2)
> - (1) escriben en EAX resultados y (2) los puede leer
> - (1) le ceden tiempo de ejecución a (2) y se quedan pausadas

a) 
Primero, hace falta definir las TSS de estas 5 tareas:
`tasks.c`:

```c

#define GDT_IDX_TASK_TAREA6 13 // :)

void tasks_init() {
    ...
    for (int i = 1; i <= 5; i++) { // inicializa 5 tareas
        task_id = create_task(TASK_TYPE);
        sched_enable_task(task_id);
    }
    gdt[GDT_IDX_TASK_TAREA6] = tss_gdt_entry_for_task(&tss_tarea6);
}
```
En `task_defines.h` habría que definir un nuevo `TASK_TAREA6_CODE_START` para que esté dentro del primer mega de memoria.

Además, habría que definir, en `tss.c`, un nuevo struct:
```c
tss_t tss_tarea6 = {
    .ss1 = 0,
    .cr3 = KERNEL_PAGE_DIR,
    .eip = TASK_TAREA6_CODE_START,
    .eflags = EFLAGS_IF,
    .esp = KERNEL_STACK,
    .ebp = KERNEL_STACK,
    .cs = GDT_CODE_0_SEL,
    .ds = GDT_DATA_0_SEL,
    .es = GDT_DATA_0_SEL,
    .gs = GDT_DATA_0_SEL,
    .fs = GDT_DATA_0_SEL,
    .ss = GDT_DATA_0_SEL, 
}
```
Ahora agregamos lo pertinente a la syscall:
En `idt.c`:
```c
void idt_init() {
    ...
    IDT_ENTRY3(80);
}
```

En `isr.h`:
```h
...
void _isr80();
```

b)
```nasm
extern enviar_resultado
...
global _isr80
_isr80:
    pushad

    push eax

    call enviar_resultado ; modificar EAX tarea 6, pausa tarea actual y devuelve selector tarea6

    add esp, 4
    
    ; hacemos el cambio a la tarea 6 :)

    mov word [sched_task_selector], ax

    jmp far [sched_task_offset]
    
    popad
    iret
```

```c
#define EAX_EN_PILA 7 // creo que está en la 7 jeje :)

uint8_t tarea_a_habilitar = -1; // var global que indicará qué tarea fue pausada por llamar a la syscall

uint16_t enviar_resultado(uint32_t eax) {
    // modificamos EAX de la tarea 6:
    modificarEAX(eax, sched_tasks[5].selector); 
    // pausamos tarea actual:
    sched_disable_task(current_task);
    tarea_a_habilitar = current_task;
    sched_enable_task(5); // activo tarea 6
    // devolvemos selector de la tarea 6:
    return sched_tasks[5].selector;
}

void modificarEAX(uint32_t nuevo_eax, uint16_t segsel) {
    tss_t* tss = obtenerTSS(segsel);
    // vamos a la pila a modificar EAX:
    tss->esp[EAX_EN_PILA] = nuevo_eax;
}
```

c) 
```nasm
    push EAX
    call procesar
    add ESP, 4
```

```c
uint32_t procesar(uint32_t res) {
    sched_enable_task(tarea_a_habilitar);
    tarea_a_habilitar = -1;
    sched_disable_task(current_task);
    return res + 32;
}
```