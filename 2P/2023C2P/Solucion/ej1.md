A) Las entradas que se agregan a la GDT que van a ser relevantes van a ser los TSS descriptors de las tareas creadas. Gracias a ellos vamos a poder pausar/ restaurar las tareas y incrementar el valor de ecx cada vez que la tarea vuelva a ser ejecutada. Tambien va a servir para comparar el valor de ecx entre las diferentes tareas.

Tambien se va a agregar a la GDT las entradas de cada tarea que va a apuntar cada una al inicio de la memoria de su codigo. 

B) Para que el valor del UTC se actualize correctamente en cada tarea lo que hay que hacer va a ser modificar el codigo de la interrupcion de reloj para que ademas de hacer lo que hacia en el taller tambien actualize el valor del registro ecx. 

Asi que el codigo de la interrupcion de reloj actualizado va a ser: 

```asm
;; Rutina de atención del RELOJ
;; -------------------------------------------------------------------------- ;;
global _isr32

_isr32:
    pushad
    ; 1. Le decimos al PIC que vamos a atender la interrupción
    call pic_finish1
    call next_clock
    ; Actualizamos el valor de ecx
    str ax
    push ax
    call actualizar_ecx
    add esp, 2
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

Y definimos en `tss.c` la funcion `actualizar_ecx`: 

```c
void actualizar_ecx(uint16_t segsel) {
    uint16_t idx = segsel >> 3;
    tss_t* tss_pointer = (tss_t*)((gdt[idx].base_15_0) | (gdt[idx].base_23_16 << 16) | (gdt[idx].base_31_24 << 24));
    uint32_t* stack_pointer = tss_pointer->esp;

    stack_pointer[5] = stack_pointer[5] + 1;
}
```

C) Para implementar el servicio `fuiLlamadaMasVeces` hay que agregar una nueva interrupcion que va a ser la syscall. Las syscalls se suelen definir a partir de la interrupcion 80 asi que vamos a usar esa. Como queremos que sea llamada desde las tareas va a ser definida como una `IDT_ENTRY3` para que pueda ser llamada desde codigo nivel 3 (el de las tareas).

Asi que en `IDT_INIT()` hay que agregar: `IDT_ENTRY3(80); // Definición de la syscall fuiLlamadaMasVeces` y en `isr.h` agregar `void _isr80()`.

D) El codigo de la interrupcion en `isr.asm` va a quedar: 


```asm
global _isr80
_isr80:
    pushad
    push ecx
    push edi
    call fuiLlamadaMasVeces
    mov [esp + 28], eax
    popad
    iret
```

Y en `sched.c` agregamos la funcion `fuiLlamadaMasVeces`:

```c
int8_t fuiLlamadaMasVeces(uint32_t id_a_comparar, uint32_t mi_utc){
  int16_t selector_tarea_a_comparar = sched_tasks[id_a_comparar].selector;

  uint32_t utc_a_comparar = obtener_utc(selector_tarea_a_comparar);

  return mi_utc > utc_a_comparar;
}
```

Y en `tss.c` agregamos la funcion `obtener_utc`: 

```c
uint32_t obtener_utc(int16_t selector_tarea_a_comparar){
    uint16_t idx = segsel >> 3;
    tss_t* tss_pointer = (tss_t*)((gdt[idx].base_15_0) | (gdt[idx].base_23_16 << 16) | (gdt[idx].base_31_24 << 24));
    uint32_t* stack_pointer = tss_pointer->esp;
    return stack_pointer[5];
}
```

E) No tiene sentido tener un registro de proposito general reservado para guardar el UTC porque la tarea se deberia encargar meticulosamente de no pisarlo en cualquier momento. Ya con llamar a una funcion de C se rompe el ECX. Una mejor idea seria que cada tarea tenga una pagina nueva que solo puede leer que cuente su UTC, y el kernel tiene mapeada esa pagina como read_write para ser el unico que la pueda actualizar. 

Asi actualizar el utc de una tarea seria actualizar el valor del dato guardado en memoria de la tarea actual y la funcion fuiLlamadaMasVeces seria comparar dos valores en memoria. 
