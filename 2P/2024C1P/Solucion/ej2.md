Lo que habria que hacer es modificar el scheduler para que checkee si la tarea es prioritaria o no y que en base a eso elija a la siguiente tarea. El codigo de atención de la interrupción va a ser exactamente el mismo. 

El codigo del scheduler va a ser: 

```c
uint16_t sched_next_task_con_prioridad(void) {
  // Buscamos la próxima tarea viva con prioridad (comenzando en la actual)
  int8_t i;
  for (i = (current_task + 1); (i % MAX_TASKS) != current_task; i++) {
    // Si esta tarea está disponible la ejecutamos
    if (sched_tasks[i % MAX_TASKS].state == TASK_RUNNABLE && es_prioritaria(sched_tasks[i % MAX_TASKS].selector)) {
      current_task = i % MAX_TASKS;
      return sched_tasks[i % MAX_TASKS].selector;
    }
  }

  // Si estamos aca es porque no se encontro ninguna tarea prioritaria a ejecutar, ahora veamos tareas no prioritarias.
  for (i = (current_task + 1); (i % MAX_TASKS) != current_task; i++) {
    // Si esta tarea está disponible la ejecutamos
    if (sched_tasks[i % MAX_TASKS].state == TASK_RUNNABLE) {
      current_task = i % MAX_TASKS;
      return sched_tasks[i % MAX_TASKS].selector;
    }
  }

  // Si estamos aca es porque no encontramos ninguna tarea distinta a la actual para correr, si la actual se puede correr devovlemos esa y si no la idle
  if (sched_tasks[i % MAX_TASKS].state == TASK_RUNNABLE) {
    current_task = i % MAX_TASKS;
    return sched_tasks[i % MAX_TASKS].selector;
  }
  // En el peor de los casos no hay ninguna tarea viva. Usemos la idle como
  // selector.
  return GDT_IDX_TASK_IDLE << 3;
}
```
Ahora lo importante es definir bien ```es_prioritaria```. En la funcion lo que vamos a hacer es acceder a edx de la tarea por medio los registros que pusheamos a la pila al entrar a la rutina de atencion del reloj usando esp para llegar alli. 

Para llegar a la pila la funcion es_prioritaria va a acceder con el selector guardado en el array al tss descriptor, del tss descriptor a la tss, de la tss a la pila y de la pila a edx. 

Lo buscamos en la pila porque desde la rutina de atencion al llamar funciones en C puede estar sobreescribiendose edx en cualquier momento, por lo cual buscar el edx que se guarda en la tss no tiene sentido. 

```c
bool es_prioritaria(uint16_t segsel){

  uint16_t idx = segsel >> 3;

  tss_t* tss_pointer = (tss_t*)((gdt[idx].base_15_0) | (gdt[idx].base_23_16 << 16) | (gdt[idx].base_31_24 << 24));

  uint32_t* stack_pointer = tss_pointer->esp;

  return stack_pointer[5] == 0xFAFAFA;
}
```





