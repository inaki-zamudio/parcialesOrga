idt.c:
```c
void idt_init() {
    ...
    IDT_ENTRY3(80);
    IDT_ENTRY3(81);
}
```

isr.h:
```h
void _isr80();
void _isr81();
```

isr.asm:
```nasm
extern malloco
...
global _isr80
_isr80:
    pushad

    push EAX ; size que le paso a malloco
    call malloco
    add esp, 4

    popad
    iret
```
El array va a ser así:
```c
typedef struct {
    uint8_t task_id;
    uint32_t inicio;
    uint32_t size;
    uint8_t desactivada;
} reserva_t;

reserva_t malloco_arr[TAMANIO_NECESARIO];

uint32_t ult = -1; // var global que indica el índice de la última reserva que registré en malloco_arr
```
```c
const uint32_t 4MB_EN_BYTES = 4000000;

void* malloco(size_t size) {
    if (reservo_mas_de_4mb(current_task, size)) return NULL;
    malloco_arr[ult + 1] = {
        task_id = current_task;
        inicio = inicio_prox_bloque(current_task);
        size = size;
        desactivada = 0;
    }
    START_DYN_MEM_PHYSICAL += size;
    return malloco_arr[ult+1].inicio;
}

uint8_t reservo_mas_de_4mb(uint8_t task_id, uint32_t size) { // chequea si EN TOTAL pidió más de 4mb, sin importar si ya los liberó o no
    uint32_t suma_total = size;
    for (uint32_t i = 0; i < TAMANIO_NECESARIO; i++) {
        if (malloco_arr[i].task_id == task_id) suma_total += malloco_arr[i].size;
    }
    return suma_total > 4MB_EN_BYTES;
}

uint32_t inicio_prox_bloque(uint8_t task_id) {
    uint32_t suma_total = 0;
    for (uint32_t i = 0; i < TAMANIO_NECESARIO; i++) {
        if (malloco_arr[i].task_id == task_id) suma_total+=malloco_arr[i].size;
    }
    return suma_total;
}
```

Ahora, veo la parte donde la tarea accede a la memoria, y por ende tengo que dársela de verdad:

```nasm
global _isr14

_isr14:
	; Estamos en un page fault.
	pushad
    ; COMPLETAR: llamar rutina de atención de page fault, pasandole la dirección que se intentó acceder
    mov ecx, cr2
    push ecx
    call page_fault_handler
    pop ecx
    cmp al, 1
    jmp .fin
    cmp al, 2
    je .dale_castigo
    .dale_castigo:
    call castigar_tarea
    .ring0_exception:
	; Si llegamos hasta aca es que cometimos un page fault fuera del area compartida.
    call kernel_exception
    jmp $
    .fin:
	popad
	add esp, 4 ; error code
	iret
```

```c
void castigar_tarea() {
    sched_disable_task(current_task);
    for (uint32_t i = 0; i < TAMANIO_NECESARIO; i++) {
        if (malloco_arr[i].task_id == current_task) {
            chau(malloco_arr[i].inicio);
        }
    }
}

Modifico la función page_fault_handler que es llamada por la interrupción de Page Fault:

```c
uint32_t START_DYN_MEM = 0xA10C0000;
uint32_t START_DYN_MEM_PHYSICAL = 0x400000;

uint8_t page_fault_handler(vaddr_t virt) {
  print("Atendiendo page fault...", 0, 0, C_FG_WHITE | C_BG_BLACK);
  // Chequeemos si el acceso fue dentro del area on-demand
  if(!(virt >= ON_DEMAND_MEM_START_VIRTUAL && virt < ON_DEMAND_MEM_END_VIRTUAL)){
    if (virt >= START_DYN_MEM && virt < inicio_prox_bloque(current_task)) {
        uint32_t cr3 = rcr3();
        uint32_t size = 0;
        for (uint32_t i = 0; i < TAMANIO_NECESARIO; i++) {
            if (malloco_arr[i].task_id == current_task) {
                size += malloco_arr[i].size;
            }
        }
        for (uint32_t i = 0; i < size / PAGE_SIZE; i++) {
            zero_page(START_DYN_MEM_PHYSICAL);
            mmu_map_page(cr3, virt, START_DYN_MEM_PHYSICAL, (MMU_P | MMU_U | MMU_W));
            virt += PAGE_SIZE;
            START_DYN_MEM_PHYSICAL += PAGE_SIZE;
        }
        return 1;
    } else {
        sched_disable_task(current_task); //"...remover la tarea del scheduler..."
        return 2;
    }
  } 
  // En caso de que si, mapear la pagina
  uint32_t cr3 = rcr3();
  mmu_map_page(cr3, virt, ON_DEMAND_MEM_START_PHYSICAL, (MMU_P | MMU_U | MMU_W));

  return 1;
}
```

Para implementar la syscall chau:

isr.asm:
```nasm
extern chau
...
global _isr81
_isr81:
    pushad

    push EAX ; ptr a liberar
    call chau
    add esp, 4

    popad
    iret
```

```c
void chau(void* ptr) {
    for (uint32_t i = 0; i < TAMANIO_NECESARIO; i++) {
        if (malloco_arr[i].inicio == ptr) {
            malloco_arr[i].desactivada = 1;
        }
    }
}
```

Ahora implementar la tarea especial:

```nasm
_isr32:
    pushad
    ; 1. Le decimos al PIC que vamos a atender la interrupción
    call pic_finish1
    call next_clock

    mov eax, [isrNumber]
    mov edi, 100
    call modulito
    add esp, 8
    cmp eax, 0
    jne .rutinaNormal

    ; si no, entonces dio múltiplo de 100 y liberamos memoria
    call desmapiar

    .rutinaNormal:
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

```c
uint32_t modulito(uint32_t a, uint32_t b) {
    return a%b;
}

uint32_t desmapiar() {
    for (uint32_t i = 0; i < TAMANIO_NECESARIO; i++) {
        if (malloco_arr[i].desactivada) {
            uint32_t cr3 = obtenerCR3(sched_tasks[malloco_arr[i].task_id].selector);
            uint32_t pages = malloco_arr[i].size / PAGE_SIZE;
            uint32_t inicio = malloco_arr[i].inicio;
            for (uint32_t i = 0; i < pages; i++) {
                mmu_unmap_page(cr3, inicio);
                inicio += PAGE_SIZE;
            }
        }
    }
}