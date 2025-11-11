Para definir las syscalls, hace falta agregar nuevas entradas en la IDT. Esto lo hacemos en `idt.c`, en la función `idt_init()`:

```c
void idt_init() {
  ...
  // queremos que las puedan llamar las tareas de usuario
  IDT_ENTRY3(80); // lock
  IDT_ENTRY3(81); // release
}
```

Luego, en `isr.asm`, vamos a declarar la rutina de atención a las interrupciones:

```c
...
void _isr80(); // lock
void _isr81(); // release
```

Finalmente, implementamos la rutina de atención en `isr.asm`:

```nasm
...
extern get_lock
...
global _isr80
_isr80:
  ; edi -> vaddr_t shared_page
  pushad
  call can_access_shared_mem
  cmp ax, 1
  je .get_lock

  ; si llegamos acá, hay que cambiar de tarea:
  call sched_next_task
  cmp ax, 0
  je .fin
  str bx
  cmp ax, bx
  je .fin
  mov word [sched_task_selector], ax
  jmp far [sched_task_offset]

.get_lock:
  push edi
  call get_lock
  add esp, 4

.fin:
  popad
  iret
```

Ahora, escribimos la función `can_access_shared_mem`:

```c
uint8_t can_access_shared_mem(vaddr_t shared_page) {
  if (lock != sched_tasks[current_task]) {
    sched_tasks[current_task].wants_lock = 1;
    sched_disable_task(current_task);
    return 0;
  }
  return 1;
}
```

Ahora, escribimos la rutina de atención para la syscall `release`:

```nasm
global _isr81
_isr81:
  ; edi -> vaddr_t shared_page
  pushad
  push edi
  call release_page
  add esp, 4
  popad
  iret
```

```c
void release_page(vaddr_t shared_page) {
  sched_tasks[current_task].wants_lock = 0;
  lock = -1;
  activate_blocked_by_lock(); // definida más abajo
}
```

Ahora, vamos a modificar la rutina de atención al page fault para que, si la tarea que quiere leer en memoria compartida está en posesión del _lock_, pueda hacerlo sin necesidad de invocar a `lock`. Para esto, vamos a mapear la página compartida si la tarea intenta acceder a ella:

```c
// esta es la función a la que llama _isr14.
bool page_fault_handler(vaddr_t virt) {
  print("Atendiendo page fault...", 0, 0, C_FG_WHITE | C_BG_BLACK);
  // Chequeemos si el acceso fue dentro del área compartida
  if (virt >= TASK_LOCKABLE_PAGE_VIRT && virt < TASK_LOCKABLE_PAGE_VIRT + PAGE_SIZE) {
    mmu_map_page(rcr3(), TASK_LOCKABLE_PAGE_VIRT, TASK_LOCKABLE_PAGE_PHY, (MMU_P | MMU_U | MMU_R));
  }
  // Chequeemos si el acceso fue dentro del area on-demand
  if(!(virt >= ON_DEMAND_MEM_START_VIRTUAL && virt < ON_DEMAND_MEM_END_VIRTUAL) {
    return false;
  }
  // En caso de que si, mapear la pagina
  uint32_t cr3 = rcr3();
  mmu_map_page(cr3, virt, ON_DEMAND_MEM_START_PHYSICAL, (MMU_P | MMU_U | MMU_W));

  return true;
}
```

> [!NOTE]
> Me falta cumplir con la parte de la consigna que dice: "Evaluar y especificar cómo se detecta si un acceso inválido es de lectura o escritura". Aparentemente hay que leer el error code en _isr14. Para una posible solución, ver [resolución de Cami](https://github.com/camigrassi04/examenesOrga/blob/main/segundos_parciales/2024C1R/solucion/ej2.md).

> [!NOTE]
> En el manual 3 de Intel, en la página 145, se especifica cómo es el error code de un page fault. El bit 1 indica si el acceso inválido fue por un intento de escritura o lectura.

> [!NOTE]
> Nota sobre la nota: `pop eax` poppea el último valor del stack y lo asigna a eax.

Ahora, tengo el problema de que si la tarea llamó a release y hay otra tarea que intentó acceder a la memoria, tengo que habilitar esa tarea. Para eso, la función de C `release` va a llamar a otra función, que voy a definir acá abajo, que recorrerá todas las tareas y pondrá en runnable todas las que tengan el campo `wants_lock == 1`.

```c
void activate_blocked_by_lock() {
  for (uint32_t i = 0; i < MAX_TASKS; i++) {
    if (sched_tasks[i].wants_lock == 1) sched_enable_task(i);
  }
}
```
```


b)

No entiendo la consigna :p
