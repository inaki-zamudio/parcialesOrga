t e  a m o
y o  t a m b i e n  t e  a m o  m i  c i e l i t o
- 5 tareas
- cuando terminan, ponen resultado en EAX

-***Syscall*** para notificar al kernel que terminó una tarea(flag).
- No se le da más CPU hasta que la 6ta tarea no termine.

-> ejecuta 6ta tarea que procesa resultados y los escribe en EAX de la tarea.

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
Por último, modificamos el struct de `sched.c`:

```c
typedef struct {
  int16_t selector;
  task_state_t state;
  uint8_t finished;
} sched_entry_t;
```

b)
```nasm
extern finalizar_tarea
...
global _isr80
_isr80:
    pushad

    call finalizar_tarea
    
    popad
    iret
```

```c
void finalizar_tarea() {
    sched_disable_task(current_task); // no le da más CPU
    sched_tasks[current_task].finished = 1; // marca flag
}
```
c)
Asumimos que las tareas fueron interrumpidas por un reloj, y entonces tienen los valores más recientes en la pila.

```c
void tarea6() { // id de la tarea 6 es 5
    uint32_t eax0 = obtenerEAX(0);
    uint32_t eax1 = obtenerEAX(1);
    uint32_t eax2 = obtenerEAX(2);
    uint32_t eax3 = obtenerEAX(3);
    uint32_t eax4 = obtenerEAX(4);
    uint32_t p = procesar(eax0, eax1, eax2, eax3, eax4);
    for (uint8_t i = 0; i < 5; i++) {
        sched_enable_task(i);
        escribirEAX(i, p);
        sched_tasks[i].finished = 0;
    }
    sched_disable_task(current_task);
}

uint32_t obtenerEAX(uint8_t task_id) {
    tss_t* tss = obtenerTSS(sched_tasks[task_id].selector);
    uint32_t esp = tss->esp;
    return esp[7]; // eax
}

uint32_t escribirEAX(uint8_t task_id, uint32_t res) {
    tss_t* tss = obtenerTSS(sched_tasks[task_id].selector);
    uint32_t esp = tss->esp;
    esp[7] = res; // escribe eax
}
```

d) 

```c
uint16_t sched_next_task(void) {
  uint8_t finished = 1;
  for (uint8_t i = 0; i < 5; i++) {
    finished = (finished & sched_tasks[i].finished);
  }
  // si alguna tarea no está terminada, no seguimos normal, y si están todas terminadas vamos a la sexta
  if (finished){
    sched_enable_task(5); // asumimos que está pausada excepto acá
    return sched_tasks[5].selector;
  }
  // Buscamos la próxima tarea viva (comenzando en la actual)
  int8_t i;
  for (i = (current_task + 1); (i % MAX_TASKS) != current_task; i++) {
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
```