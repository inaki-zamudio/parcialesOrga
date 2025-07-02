a) `swap` bloqueante.
Para definir la syscall `swap` vamos a definir una entrada en la IDT:

```C
void idt_init() {
    ...
    IDT_ENTRY3(100);
}
```
Elegimos el número de interrupción `100` ya que no es uno de los reservados por Intel ni por hardware.

Luego, debe ser una entry de nivel 3 ya que cualquier tarea debe poder llamarla.

Declaramos la interrupción en `isr.h`:
```h
void _isr100();
```

Ahora definimos en `isr.asm` la rutina de interrupción:

*Asumo que se pasa el id de la tarea destino por `al`*

```nasm
global _isr100
_isr100:
    pushad

    push eax
    call swap_bloqueante ; handler en C
    add esp, 4 

    cmp al, 0
    jne .fin ; si se pudo hacer el swap, terminamos

    ; forzamos el context switch a otra tarea
    call sched_next_task
    
    mov word [sched_task_selector], ax
    jmp far [sched_task_offset]
    
.fin:
    popad
    iret
```
Extendemos el struct que describe a las tareas para el scheduler:

```C
typedef struct {
  int16_t selector;
  task_state_t state;
  uint8_t wants_to_swap_with; // cuando agregamos a una tarea este campo es -1, ya que no pertenece al índice de ninguna tarea
} sched_entry_t;

```

```C
uint8_t swap_bloqueante(uint8_t id_tarea_destino) {
    if (sched_tasks[id_tarea_destino].wants_to_swap_with = current_task){
        // obtenemos las tss de las tareas
        tss_t* tss_tarea_actual = obtener_tss(sched_tasks[id_tarea_destino].selector);
        tss_t* tss_tarea_destino = obtener_tss(sched_tasks[current_task].selector);

        // buscamos la pila de cada tarea, ya que ahí es donde se guardan los valores recientes de los registros y donde vamos a querer pisarlos para que se actualicen

        // para la tarea que no se encuentra en ejecución, buscamos el esp en la tss y nos movemos por la pila de nivel 0 de acuerdo al offset de cada registro

        // para la tarea actual, tenemos guardado en la tss el valor de la base de la pila de nivel 0 (que nunca cambia). Por lo que la vamos a usar para movernos en la pila de nivel 0 actual (desde la base tenemos que restar para movernos por la pila)

        uint32_t* esp_tarea_destino = tss_tarea_destino->esp;
        uint32_t* base_pila_tarea_actual = tss_tarea_actual->ebp;

        // guardamos una copia de los registros de la tarea destino
        uint32_t edi = esp_tarea_destino[0];
        uint32_t esi = esp_tarea_destino[1];
        uint32_t ebx = esp_tarea_destino[4];
        uint32_t edx = esp_tarea_destino[5];
        uint32_t ecx = esp_tarea_destino[6];
        uint32_t eax = esp_tarea_destino[7];

        // seteamos los valores de los registros de la tarea destino a los de la actual
        esp_tarea_destino[0] = base_pila_tarea_actual - 48;
        esp_tarea_destino[1] = base_pila_tarea_actual - 44;
        esp_tarea_destino[4] = base_pila_tarea_actual - 34;
        esp_tarea_destino[5] = base_pila_tarea_actual - 30;
        esp_tarea_destino[6] = base_pila_tarea_actual - 26;
        esp_tarea_destino[7] = base_pila_tarea_actual - 22;

        // ahora seteamos los de la pila de la tarea actual
        *(base_pila_tarea_actual - 48) = edi;
        *(base_pila_tarea_actual - 44) = esi;
        *(base_pila_tarea_actual - 34) = ebx;
        *(base_pila_tarea_actual - 30) = edx;
        *(base_pila_tarea_actual - 26) = ecx;
        *(base_pila_tarea_actual - 22) = eax;

        // como ya se realizó el swap, limpiamos el valor "wants_to_swap_with"
        sched_tasks[id_tarea_destino].wants_to_swap_with = -1;
        sched_tasks[current_task].wants_to_swap_with = -1; 

        return 1; // se pudo realizar el swap
    } else {
        sched_disable_task(current_task); // pausamos tarea actual
        sched_tasks[current_task].wants_to_swap_with = id_tarea_destino; // marcamos que desea swappear con la tarea destino y se queda esperando a que esta esté disponible para el swap
        return 0; // no se pudo hacer el swap
    }
}
```
Asumimos que la pila de nivel 0 se ve así:
| Registro   | Dirección | Comentarios                        |
|------------|-----------|------------------------------------|
| **SS_3**   |           | Acá apunta el ebp                  |
| **ESP_3**  |           |                                    |
| **EFLAGS** |           |                                    |
| **CS_3**   |           |                                    |
| **EIP_3**  |           |                                    |
| **EAX**    | +0x1C     |                                    |
| **ECX**    | +0x18     |                                    |
| **EDX**    | +0x14     | [5]                                |
| **EBX**    | +0x10     | [4]                                |
| **ESP**    | +0x0C     | [3]                                |
| **EBP**    | +0x08     | [2]                                |
| **ESI**    | +0x04     | [1]                                |
| **EDI**    | +0x00     | [0] <─ esp al momento del `jmp far`|

