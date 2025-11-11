> Características del sistema:
> - 5 tareas nivel 3
> - 1 tarea nivel 0
> - Alguna de las de nivel 3 le escribe un resultado a la de nivel 0 mediante una syscall, y les ceden su tiempo de ejecución (i.e., se salta a la tarea de nivel 0 antes que caiga la interrupción del reloj.

a)

Entonces, hay que:
- Definir las 5 tareas de nivel 3. (1)
- Definir la tarea de nivel 0. (2)
- Agregar a la IDT la entrada para la nueva syscall. (3)

(1)
En la función tasks_init() de tasks.c, hay que crear cada tarea y agregarla al scheduler:
```c
void tasks_init(void) {
    ...
    for (int i = 1; i <=5; i++) {
        task_id = create_task(TASK_TYPE);
        sched_enable_task(task_id);
    }
}
```

(2)
En tasks.c, defino una nueva función `static int8_t create_kernel_task(tipo_e tipo) similar a create_task, pero para crear tareas de nivel de kernel.

```c
int8_t create_kernel_task(paddr_t code_start) {
  size_t gdt_id;
  for (gdt_id = GDT_TSS_START; gdt_id < GDT_COUNT; gdt_id++) {
    if (gdt[gdt_id].p == 0) {
      break;
    }
  }
  kassert(gdt_id < GDT_COUNT, "No hay entradas disponibles en la GDT");

  int8_t task_id = sched_add_task(gdt_id << 3);
  tss_tasks[task_id] = tss_create_priority_task(code_start);
  gdt[gdt_id] = tss_gdt_entry_for_task(&tss_tasks[task_id]);

  return task_id;
}

    vaddr_t stack = KERNEL_STACK;
    return (tss_t){
        .cr3 = cr3,
        .esp = stack,
        .ebp = stack,
        .eip = code_start,
        .cs = GDT_CODE_0_SEL,
        .ds = GDT_DATA_0_SEL,
        .es = GDT_DATA_0_SEL,
        .fs = GDT_DATA_0_SEL,
        .gs = GDT_DATA_0_SEL,
        .ss = GDT_DATA_0_SEL,
        .ss0 = GDT_DATA_0_SEL,
        .esp0 = stack,
        .eflags = EFLAGS_IF
    };
}
```
(3)

En idt.c:

```c
void idt_init() {
    ...
    IDT_ENTRY3(80);
```

En isr.h:

```h
...
void _isr80();
```

b)

En isr.asm:

```nasm
global _isr80
_isr80:
    pushad
    push EAX
    call enviar_resultado ; modifica EAX de tarea 6, pausa tarea actual y devuelve selector de tarea 6 
    add esp, 4 

    mov word [sched_task_selector], ax

    jmp far [sched_task_offset]

    popad
    iret
```

```c
uint16_t enviar_resultado(uint32_t eax) {
    modificarEAX(sched_tasks[5].selector, eax); // sched_tasks[5] es la tarea 6
    sched_disable_task(current_task);
    sched_enable_task(5); // activo tarea 6
    return sched_tasks[5].selector;
}

void modificarEAX(uint16_t segsel, uint32_t nuevo_eax) {
    tss_t* tss = obtenerTSS(segsel);
    tss->esp[7] = nuevo_eax;
}

tss_t* obtenerTSS(uint16_t segsel) {
    uint16_t idx = selector >> 3;
    return gdt[idx].base; 
}
```


