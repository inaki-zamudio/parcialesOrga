Cuando se ejecute la rutina de atención a la interrupción del reloj, en vez de buscar la próxima tarea a ejecutarse como lo hacíamos en el taller, vamos a revisar primero si hay alguna otra tarea prioritaria. Tenemos que asegurarnos de:
- Definir dónde se guarda el EDX de nivel de usuario de las tareas desalojadas por el scheduler
- Precisar cómo el scheduler determina que una tarea es prioritaria

Para agregar esta funcionalidad, es necesario modificar la rutina de atención de la interrupción del reloj:

```nasm
global _isr32
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

Y es `sched_next_task` la función que queremos modificar para que tenga la capacidad de elegir tareas prioritarias:

```c
int8_t last_task_priority = 0;
int8_t last_task_no_priority = 0;

uint16_t sched_next_task(void) {
  // Buscamos la próxima tarea viva (comenzando en la actual)
  int8_t i;
  for (i = (last_task_priority + 1); (i % MAX_TASKS) != last_task_priority; i++) {
    // Si esta tarea está disponible la ejecutamos
    if (sched_tasks[i % MAX_TASKS].state == TASK_RUNNABLE && esPrioritaria(i)) {
      break;
    }
  }

  // Ajustamos i para que esté entre 0 y MAX_TASKS-1
  i = i % MAX_TASKS;

  // Si la tarea que encontramos es ejecutable entonces vamos a correrla.
  if (sched_tasks[i].state == TASK_RUNNABLE && esPrioritaria(i)) {
    current_task = i;
    return sched_tasks[i].selector;
  }

  for (i = (last_task_no_priority + 1); (i % MAX_TASKS) != last_task_no_priority; i++) {
    // Si esta tarea está disponible la ejecutamos
    if (sched_tasks[i % MAX_TASKS].state == TASK_RUNNABLE) {
      break;
    }
  }

  // Ajustamos i para que esté entre 0 y MAX_TASKS-1
  i = i % MAX_TASKS;

  // Si la tarea que encontramos es ejecutable entonces vamos a correrla.
  if (sched_tasks[i].state == TASK_RUNNABLE) {
    current_task = i;
    return sched_tasks[i].selector;
  }

  // En el peor de los casos no hay ninguna tarea viva. Usemos la idle como
  // selector.
  return GDT_IDX_TASK_IDLE << 3;
}

uint8_t esPrioritaria(uint8_t task_id) {
  tss_t* tss = obtener_tss(current_task[task_id]);
  return tss->esp[5] == 0x00FAFAFA;
}

tss_t* obtener_TSS(uint16_t selector){
    uint16_t idx = selector >> 3;
    return gdt[idx].base;
}
```


