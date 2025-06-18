A) Lo primero que voy a hacer es modificar el struct `sched_entry_t` en `sched.c` para informar si la tarea esta esperando el lock. Cuando agregamos una tarea al scheduler la vamos a agregar con ese atributo nuevo en 0. 

```c
typedef struct {
  int16_t selector;
  task_state_t state;
  int8_t wants_lock;
} sched_entry_t;
```

Para agregar las dos syscalls nuevas vamos a definir dos interrupciones nuevas en `idt_init()`.

Entonces en `idt_init()` agregamos: 

```c
IDT_ENTRY3(80);
IDT_ENTRY3(81);
```

Como las syscalls suelen definirse a partir del numero de interrupcion 80 vamos a definir lock como la 80 y release como la 81.

Para que puedan ser llamadas desde las tareas ambas van a ser `IDT_ENTRY3`. Tambien en `isr.h` hay que agregar:

```h
void _isr80();
void _isr81();
```

La consigna dice que ambas syscalls reciben como parametro la direccion virtual de la página compartida. Como la consigna no especifica como recibe ese dato la syscall __asumo que lo va a recibir en el registro ecx de la tarea que llamo a la syscall__.

Ahora defino en `isr.asm` la rutina de atencion de lock:

```asm
global _isr80
_isr80:
    pushad
    call esta_disponible_el_lock ; Devuelve 1 si nadie esta usando el lock o si el que lo usa es la tarea actual. Si el que lo usa es la tarea actual tambien se va a volver a mapear (preguntar lo de volver a mapear)
    cmp ax 0
    je .pausar_tarea
    ; Si estamos aca es porque se puede usar el lock.

    str ax
    push ax
    call obtener_ecx ; Obtengo el valor de la direccion virtual que se quiere acceder
    add esp, 2

    push eax
    call get_lock
    add esp, 4
    jmp fin

    .pausar_tarea:
    ; Si estamos aca es porque el lock ya esta en uso por otra tarea, asi que tenemos que pausar la tarea, pedirle al scheduler la siguiente tarea y saltar a esa tarea
    call pausar_tarea_actual
    call sched_next_task
    cmp ax, 0
    je .fin

    str bx
    cmp ax, bx
    je .fin

    mov word [sched_task_selector], ax
    jmp far [sched_task_offset] 
    ; Si estamos aca es porque esta disponible el lock y es nuestro turno de agarrarlo. 
    str ax
    push ax
    call obtener_ecx ; Obtengo el valor de la direccion virtual que se quiere acceder
    add esp, 2

    push eax
    call get_lock
    add esp, 4

    .fin:
        popad
        iret
```

Y defino las funciones auxiliares en `sched.c`:

```c

int8_t esta_disponible_el_lock(){
    return (task_with_lock == -1 || task_with_lock == current_task);
}

void pausar_tarea_actual(){
    sched_tasks[current_task].wants_lock = 1;
}

```

Y en `tss.c` agrego la funcion:
```c
pd_entry_t* obtener_ecx(uint16_t segsel) {
    uint16_t idx = segsel >> 3;
    tss_t* tss_task = (tss_t*)((gdt[idx].base_15_0) | (gdt[idx].base_23_16 << 16) | (gdt[idx].base_31_24 << 24));
    uint32_t* pila = tss_task->esp;
    uint32_t ecx = pila[6];
    return ecx;
}
```

Tambien habria que modificar el codigo de `sched_next_task` para que tenga en cuenta que no puede ejecutar tareas que quieren acceder al lock hasta que este no este disponible. 

```c
uint16_t sched_next_task(void) {
  // Buscamos la próxima tarea viva (comenzando en la actual)
  int8_t i;
  for (i = (current_task + 1); (i % MAX_TASKS) != current_task; i++) {
    if(task_with_lock == -1){
        // Si el lock esta disponible con que sea runnable se puede ejecutar
        if (sched_tasks[i % MAX_TASKS].state == TASK_RUNNABLE) {
            break;
        }
    }
    else{
        // Si el lock no esta disponible solo se pueden acceder a las tareas que no lo solicitaron
        if (sched_tasks[i % MAX_TASKS].state == TASK_RUNNABLE && sched_tasks[i % MAX_TASKS].wants_lock == 0) {
            break;
        }
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
La syscall release tambien va a recibir por ecx la direccion virtual.

No esta muy clara la consigna en que pasa en casos que esa direccion virtual no es la direccion virtual real de la pagina compartida. Lo que voy a hacer es simplemente desmapear la pagina que me pasan por parametro y luego modificar la variable global indicando que esta disponible el lock. 

Ahora defino la rutina de atencion para release en `isr.asm`:

```asm
global _isr80
_isr80:
    pushad
    str ax
    push ax
    call obtener_ecx ; Obtengo el valor de la direccion virtual que se quiere desmapear
    add esp, 2
    mov ecx, cr3
    push eax ; Pusheo la direccion virtual a desmapear
    push ecx ; Pusheo el cr3
    call mmu_unmap_page 
    add esp, 8
    call liberar_lock
    popad
    iret
```

Y en `sched.c` defino la funcion `liberar_lock`:

```c
void liberar_lock(){
    sched_tasks[i].wants_lock = 0;
    task_with_lock = -1;
}
```

Para hacer que la tarea pueda leer si no hay ninguna tarea con lock lo que va a tener que ser modificado es la interrupcion de la page fault. 

Cuando se produce una page fault en cr2 se guarda la direccion virtual a la que se trato de acceder que produjo el fault, y se pushea un codigo de error al tope de la pila. 

Si el codigo de error tiene el segundo bit en 0 es porque se trato de leer. 

Va a quedar indefinido el comportamiento si tratan de escribir, va a ir directo al final, lo mismo si trata de leer y el lock esta ocupado. Ya que la consigna no dice nada y el ultimo punto habla de que pasa si la tarea lo quiere escribir. 

```asm
;; Rutina de atención de Page Fault
;; -------------------------------------------------------------------------- ;;
global _isr14

_isr14:
	; Estamos en un page fault.
	pushad 
    mov edi, cr2 ; En cr2 por convencion esta la direccion lineal que produjo el fault
    pop eax ; Desapilamos de la pila el codigo de error 
    ; Le sacamos el offset a la direccion virtual que se trato de acceder para solo comparar la base de la pagina
    shr edi, 12
    shl edi, 12
    cmp edi, TASK_LOCKABLE_PAGE_VIRT
    jne .normal
    ; Si estamos aca es porque se quiso leer o escribir en la pagina 
    ; Movemos eax para que quede solamente el dato del bit write
    and eax, 2
    cmp eax, 1 ; Si se quiso escribir vamos al final
    ; Si estamos aca es porque se quizo leer. 
    call lock_disponible
    cmp ax, 0
    je .fin
    ; Si estamos aca es porque el lock esta disponible.
    mov eax, cr3
    mov ecx, [TASK_LOCKABLE_PAGE_PHY]
    mov edi, [TASK_LOCKABLE_PAGE_VIRT]
    mov edx, [READ_ONLY_USER_ATTRIBUTES]
    push edx
    push edi
    push ecx 
    push eax
    call mmu_map_page
    add esp, 16 ; Desapilo
    jmp .fin
    .normal: 
        push edi ; Pusheamos la direccion lineal donde se produjo la excepcion
        call page_fault_handler
        add esp, 4
        cmp al, 1
        je .fin
    .ring0_exception:
        call kernel_exception
        jmp $
    .fin:
      popad
      add esp, 4 ; error code 
      iret
```

Y agregamos la funcion `lock_disponible` en `sched.c`: 

```c
int8_t lock_disponible(){
    return task_with_lock == -1;
}
```

B) Si dejan de existir las syscalls habria que trabajar sobre la interrupcion por page fault y el scheduler. 

Para que la tarea solicite el lock directamente escribiendo/ leyendo en memoria habria que extraer la gran mayoria del codigo de la syscall y pegarlo en la rutina del pagefault si se quizo escribir en la compartida. 

Para que el lock se le saque a la tarea despues de 5 desalojos lo que tendria que ver es primero agregar una variable global que se llame cantidad_desalojos_tarea_con_lock que empieza en 0 y que cada vez que en next_task se desaloja a la tarea con el lock se le suma en 1 esa variable. Una vez que llega a 5 le sacamos el lock. Cuando a una nueva tarea le damos el lock le volvemos a poner 0 a esta variable. 
