a)

La idea para la implementación de la syscall va a ser:
Cuando la `tarea_i` invoca a `swap(tarea_j)`, pueden ocurrir dos cosas:
- Si la  `tarea_j` no llamó a `swap`, vamos a pausar/bloquear (i.e. con el estado que ya está en el sistema, `TASK_PAUSED`, o bien con un estado nuevo `TASK_BLOCKED`, todavía no lo decidí) a la `tarea_i`.
- Si la `tarea_j` llamó a swap, vamos a hacer el intercambio de registros.

```nasm
global _isr80
_isr80:
  ; edi -> uint8_t task_id
  pushad
  push edi
  push esp
  call swap
  add esp, 8
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

```c
typedef struct {
  int16_t selector;
  task_state_t state;
  uint8_t swap = -1; // almacena el id de la tarea que fue argumento al llamar a swap
} sched_entry_t;

bool swap(uint8_t task_id, uint32_t pila_current_task) {
  if (sched_tasks[task_id].swap != current_task) { // si no quiere swap, o quiere pero con otra tarea
    sched_tasks[current_task].swap = task_id; // quiere swap con la task_id
    sched_disable_task(current_task); // deshabilito la tarea, o debería bloquearla?
    return 0; // 0 si no se pudo swappear
  }
  // si estamos acá, la tarea task_id quería hacer swap con la current_task
  sched_enable_task(task_id); // sabíamos que estaba pausada, la activamos
  swap_registers(current_task, task_id, pila_current_task);
  return 1;
}
```

Ahora, defino la función `swap_registers`:
Sé que la `t2` quedó pausada luego de haber llamado a `swap()` porque en la rutina de atención a la interrupción, se hizo un `jmp far`, entonces se guardaron los registros _de nivel de kernel_ en la TSS. Como el cambio de contexto se dio antes del popad, en la pila de nivel 0 de la `t2` tenemos los registros actualizados de nivel de usuario, que es lo que nos piden, por lo que vamos a acceder a esos y pisar los registros de la pila de la `t1` con los de la pila de la `t2`.

```c
void swap_registers(uint8_t t1, uint8_t t2, uint32_t pila_t1) {
  tss_t* pila_t2 = obtenerTSS(sched_tasks[t2].selector)->esp;
  
  uint32_t edi = pila_t2[0];
  uint32_t esi = pila_t2[1];
  uint32_t ebx = pila_t2[4];
  uint32_t edx = pila_t2[5];
  uint32_t ecx = pila_t2[6];
  uint32_t eax = pila_t2[7];

  // seteamos los valores de la tarea t1 en la t2
  pila_t2[0] = pila_t1[0];
  pila_t2[1] = pila_t1[1];
  pila_t2[4] = pila_t1[4];
  pila_t2[5] = pila_t1[5];
  pila_t2[6] = pila_t1[6];
  pila_t2[7] = pila_t1[7];

  // le pongo a t1 los registros de la t2
  pila_t1[0] = edi;
  ...

  // limpiamos el valor de sched_entry_t.swap
  sched_tasks[t1].swap = -1;
  sched_tasks[t2].swap = -1;
}
```

b)

Se pide programar la syscall `swap_now`, la cual va a tener un comportamiento parecido a `swap`, pero en vez de quedarse pausada, en el código de la interrupción voy a saltar a alguna próxima tarea. Una vez vuelva a la ejecución de la tarea que llamó a `swap_now`, voy a limpiar el flag `swap` y hacer `popad` e `iret`.

```nasm
global _isr81
_isr81:
  pushad
  push edi
  push esp
  call swap_now
  add esp, 8
  call sched_next_task

  cmp ax, 0
  je .fin

  str bx
  cmp ax, bx
  je .fin

  mov word [sched_task_selector], ax

  jmp far [sched_task_offset]

  call clear_swap

  popad
  iret
```

```c
bool swap(uint8_t task_id, uint32_t pila_current_task) {
  if (sched_tasks[task_id].swap != current_task) { // si no quiere swap, o quiere pero con otra tarea
    sched_tasks[current_task].swap = task_id; // quiere swap con la task_id
    return 0; // 0 si no se pudo swappear
  }
  // si estamos acá, la tarea task_id quería hacer swap con la current_task
  sched_enable_task(task_id); // sabíamos que estaba pausada, la activamos
  swap_registers(current_task, task_id, pila_current_task);
  return 1;
}

void clear_swap() {
  sched_tasks[current_task].swap = -1;
}
```


Por último, por completitud, vamos a agregar las entradas necesarias a la IDT para estas nuevas interrupciones:

### `idt.c`
```c
...
void idt_init() {
  ...
  IDT_ENTRY3(80);
  IDT_ENTRY3(81);
}
```

Luego, voy a declarar las rutinas de atención a las interrupciones:

### `isr.h`
```c
...
void _isr80();
void _isr81();
```
