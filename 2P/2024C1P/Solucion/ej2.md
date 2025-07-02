Modifico la `sched_next_task`:

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
    last_task_priority = i;
    return sched_tasks[i].selector;
  }

  for (i = (last_task_no_priority + 1); (i % MAX_TASKS) != last_task_no_priority; i++) {
    // Si esta tarea está disponible la ejecutamos
    if (sched_tasks[i % MAX_TASKS].state == TASK_RUNNABLE) {
      break;
    }
  }

    // Si la tarea que encontramos es ejecutable entonces vamos a correrla.
  if (sched_tasks[i].state == TASK_RUNNABLE) {
    last_task_no_priority = i;
    return sched_tasks[i].selector;
  }

  // En el peor de los casos no hay ninguna tarea viva. Usemos la idle como
  // selector.
  return GDT_IDX_TASK_IDLE << 3;
}

uint8_t esPrioritaria(uint8_t index) {
    jau tu
}
```