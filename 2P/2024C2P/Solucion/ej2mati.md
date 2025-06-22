A) Para definir la interrupción 40 hay que en `idt.c` agregar `IDT_ENTRY0(40); ` Para definir la interrupción de hardware del cartucho. Y en `isr.h` definir `void isr40();` y luego en `isr.asm` definir la rutina de atención de la interrupción.

Definimos en `isr.asm`:

```asm
;; Rutina de atención del lector de cartuchos
;; -------------------------------------------------------------------------- ;;
global _isr40

_isr40:
    pushad
    ; Le decimos al PIC que vamos a atender la interrupción
    call pic_finish1
    call deviceready
    popad
    iret
```

Y en `sched.c` modificamos el struct `sched_entry_t`: 

```c
typedef struct {
  int16_t selector;
  task_state_t state;
  int8_t esta_pausada_por_open; // 0 o 1, se va a setear en 1 cuando se haga opendevice y se va a setear en 0 en el primer isr40 y en close device
  int8_t tipo_acceso_memoria_buffer; // Puede ser 0 (no accede), 1 (accede por dma) o 2 (accede por copia)
  paddr_t paddr_copia; // Todas las tareas cuando se definen van a definir cual va a ser la direccion fisica en la cual va a estar la copia del buffer en caso de que lo pidan
  vaddr_t vaddr_copia; // Si es por copia al hacer opendevice se va a sobreescribir este valor
} sched_entry_t;
```
Los atributos extra que le agregue al struct van a ser todos seteados en cero en la funcion `sched_add_task` cuando se agrega la tarea al array del scheduler. El unico valor que va a ser seteado va a ser el de `paddr_copia`, por lo que habria que hacer que la funcion add_task tambien reciba como parametro una direccion fisica.


Y agregamos la funcion `deviceready`:

```c
void deviceready(){
  for(int8_t i = 0; i < MAX_TASKS; i++){
    pd_entry_t* pd = obtener_cr3(sched_tasks[i].selector);
    if(sched_tasks[i].esta_pausada_por_open){
      // Si accede por dma y esta pausada porque viene de hacer open hay que mapearle la memoria
      if(sched_tasks[i].tipo_acceso_memoria_buffer == 1){
        buffer_dma(pd);
      }
      sched_tasks[i].esta_pausada_por_open = 0; 
      sched_tasks[i].state = TASK_RUNNABLE;
    }
    if(sched_tasks[i].tipo_acceso_memoria_buffer == 2){
      buffer_copy(pd, sched_tasks[i].paddr_copia, sched_tasks[i].vaddr_copia);
    }
  }
}
```

Y en `tss.c` exportamos la gdt y agregamos la función: 

```c
pd_entry_t* obtener_cr3(uint16_t segsel) {
    uint16_t idx = segsel >> 3;
    tss_t* tss_pointer = (tss_t*)((gdt[idx].base_15_0) | (gdt[idx].base_23_16 << 16) | (gdt[idx].base_31_24 << 24));
    return tss_pointer.cr3;
}
```

B) Para agregar las dos syscalls nuevas vamos a definir dos interrupciones nuevas en `idt_init()`.

Como las syscalls suelen definirse a partir del numero de interrupcion 80 vamos a definir opendevice como la 80 y closedevice como la 81. 

Para que puedan ser llamadas desde las tareas ambas van a ser `IDT_ENTRY3`. Tambien en `isr.h` hay que agregar: 

```h
void _isr80();
void _isr81();
```

En `isr.asm` definimos la rutina de atencion de la interrupcion numero 80:

```asm
global _isr80
_isr80:
    pushad
    ; Primero obtenemos el dato ubicado en la posicion de memoria 0xACCE50. Como el mapa de memoria en la interrupcion es el mismo de la tarea podemos leer esa direccion sin necesidad de mapearla. 
    mov al, [0xACCE50]
    cmp al, 0
    je .acceso_en_0
    cmp al, 1
    je .dma
    ; Si estamos aca es porque quiere acceder por copia
    .copia:
      str ax
      push ax 
      call obtener_ecx
      add esp, 2
      push eax
      call opendevice_set_copia
      add esp, 4
      jmp .fin
    .dma: 
      call opendevice_set_dma
      jmp .fin
    .acceso_en_0:
      call opendevice_pause
    .fin: 
      ; Realizamos el cambio de tareas 
      call sched_next_task
      cmp ax, 0
      je .recontra_fin

      str bx
      cmp ax, bx
      je .fin

      mov word [sched_task_selector], ax
      jmp far [sched_task_offset] 

      .recontra_fin:
        popad
        iret
```

En `tss.c` agregamos la auxiliar: 

```c
pd_entry_t* obtener_ecx(uint16_t segsel) {
    uint16_t idx = segsel >> 3;
    tss_t* tss_task = (tss_t*)((gdt[idx].base_15_0) | (gdt[idx].base_23_16 << 16) | (gdt[idx].base_31_24 << 24));
    uint32_t* pila = tss_task->esp;
    uint32_t ecx = pila[6];
    return ecx;
}
```

En `sched.c` agregamos las auxiliares: 

```c
void opendevice_set_copia(vaddr_t vaddr_copia){
  sched_tasks[current_task].esta_pausada_por_open = 1;
  sched_tasks[current_task].state = TASK_PAUSED;
  sched_tasks[current_task].vaddr_copia = vaddr_copia;
  sched_tasks[current_task].tipo_acceso_memoria_buffer = 2;
}

void opendevice_set_dma(){
  sched_tasks[current_task].esta_pausada_por_open = 1;
  sched_tasks[current_task].state = TASK_PAUSED;
  sched_tasks[current_task].tipo_acceso_memoria_buffer = 1;
}

void opendevice_pause(){
  sched_tasks[current_task].esta_pausada_por_open = 1;
  sched_tasks[current_task].state = TASK_PAUSED;
}
```

Por ultimo definimos la interrupcion numero 81 de closedevice: 

```asm
global _isr80
_isr80:
    pushad
    call desnotificar
    popad 
    iret

```

Y en `sched.c` agregamos la funcion desnotificar: 

```c
void desnotificar(){
    if(sched_tasks[current_task].esta_pausada_por_open){
        sched_tasks[current_task].state = TASK_RUNNABLE;
    }
    sched_tasks[current_task].esta_pausada_por_open = 0;
    sched_tasks[current_task].vaddr_copia = 0;
    sched_tasks[current_task].tipo_acceso_memoria_buffer = 0;
}
```
