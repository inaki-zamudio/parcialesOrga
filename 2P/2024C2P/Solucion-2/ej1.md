# Características del sistema que se pide:
- El lector copia los gráficos del cartucho a un buffer de video de 4KiB en memoria
- Si el buffer está lleno, el lector se lo informa al kernel con la IRQ 40
- Las tareas quieren acceder a la pantalla
- Lo hacen de dos formas:
  - DMA (Direct Memory Access): mapea la virtual 0xBABAB000 directamente al buffer de video (0xF151C000)
  - Por copia: Se copia el buffer en una página física específica (por cada tarea) y se mapea en alguna dir virtual Tareas solicitan acceso al buffer mediante syscall `opendevice`, una vez ya configuraron el tipo de acceso al buffer en la virtual 0xACCE5000 (r/w para la tarea). Allí hay una variable uint8_t acceso, que puede ser:
  - 0 -> no accede al buffer
  - 1 -> DMA
  - 2 -> Por copia. En este caso, la virtual donde realizar la copia está dada por ECX al momento de llamar a `opendevice`, y va a tener r/w. **Se asume que las tareas tienen esa dirección virtual mapeada a alguna dirección física**.
- La tarea no retoma ejecución hasta que el buffer esté listo, y se haya hecho el mapeo DMA o la copia.
- Cuando termine de usar el buffer, lo indica mediante `closedevice`. En esta se debe retirar el acceso al buffer que corresponda.
- La interrupción de buffer completo se encarga de dar acceso correspondiente a las tareas que lo hayan solicitado y actualizar las copias del buffer "vivas".
- Cada tarea que accede por copia debería mantener una única copia, idealmente.

### Ejercicio 1:

a)

Me piden programar la rutina que atenderá la interrupción generada por el lector de cartuchos al llenar el buffer.

Antes que nada, voy a definir una nueva entrada en la idt para la IRQ 40:

### `idt.c`
```c
void idt_init() {
  ...
  IDT_ENTRY0(40); // es de nivel 0, ya que es una interrupción de hardware.
}
```

Luego, voy a declarar la rutina de atención a la interrupción 40:

### `isr.h`
```c
...
void _isr40(); // cartucho
```

Ahora voy a definir la rutina de atención a la interrupción:

### `isr.asm`
```nasm
...
extern deviceready
...
global _isr40
_isr40:
  pushad
  call pic_finish1 ; le decimos al PIC que vamos a atender a la interrupción 
  call deviceready
  popad
  iret
```

### `cartucho.c`
```c
void deviceready() {
  for (uint32_t i = 0; i < MAX_TASKS; i++) {
    sched_entry_t tarea = sched_tasks[i];
    if (tarea.mode == NO_ACCESS) {
      continue;
    } 
    if (tarea.status == TASK_BLOCKED) {
      if (tarea.mode == DMA_ACCESS) {
        buffer_dma(obtenerCR3(tarea.selector));
      }
      if (tarea.mode == COPY_ACCESS) {
        buffer_copy(obtenerCR3(tarea.selector), mmu_next_user_page(), tarea.copyDir));
      }
      tarea.status = TASK_RUNNABLE;
    } else { // si no está bloqueada
      if (tarea->mode == COPY_ACCESS) {
        paddr_t destino = obtenerDireccionFisica(obtenerCR3(tarea.selector), tarea->copyDir);
        copy_page(xF151C000, destino)
      }
    }
  }
}
```

También es importante agregar, en el enum `task_state_t`, el nuevo estado `BLOCKED`, que implicará que fue bloqueada por pedir acceder al buffer:

### `sched.c`
```c
typedef enum {
  TASK_SLOT_FREE,
  TASK_RUNNABLE,
  TASK_PAUSED,
  TASK_BLOCKED
} task_state_t;
```

Y además agrego dos campos nuevos en `sched_entry_t`:

### `sched.c`
```c
typedef struct {
  int16_t selector;
  task_state_t state;
  // Agregado para resolver
  // el mecanismo propuesto
  uint32_t copyDir;
  uint8_t mode;
} sched_entry_t;
```

b)

Voy a agregar las nuevas entradas en la IDT, y declarar la rutina de atención a las interrupciones:

### `idt.c`
```c
void idt_init() {
  ...
  IDT_ENTRY3(80); // opendevice
  IDT_ENTRY3(81); // closedevice
}
```

### `isr.h`
```c
...
void _isr80();
void _isr81();
```

Luego, voy a escribir el código de las interrupciones:

### `isr.asm`
```nasm
...
extern opendevice
...
global _isr80
_isr80:
  pushad
  push ecx
  call opendevice
  add esp, 4
  popad
  iret
```

```c
void opendevice(uint32_t copyDir) {
  sched_entry_t tarea = sched_tasks[current_task];
  tarea.status = TASK_BLOCKED;
  tarea.mode = *0xACCE5000; // accedo a la memoria virtual
  tarea.copyDir = copyDir;
}
```

### `isr.asm`
```nasm
...
extern closedevice
...
global _isr81
_isr81:
  pushad
  call closedevice
  popad
  iret
```

```c
void closedevice() {
  sched_entry_t tarea = sched_tasks[current_task];
  if (tarea.mode == ACCESS_DMA) {
    mmu_unmap_page(rcr3(), 0xBABAB000);
  }
  if (tarea.mode == ACCESS_COPY) {
    mmu_unmap_page(rcr3(), tarea.copyDir);
  }

  tarea.mode = NO_ACCESS;
}
```
