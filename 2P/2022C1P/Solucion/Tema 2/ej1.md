1) Para agregar la syscall nueva vamos a definir una interrupcion nueva en la idt modificando `idt_init()`.

Como las syscalls suelen definirse a partir del numero de interrupcion 80 va a ser la numero 80.

Como la interrupcion tiene que poder ser llamada desde el codigo de las tareas nivel 3 vamos a definirla como `IDT_ENTRY3`.

Entonces en la funcion `idt_init()` agrego: 

```c
IDT_ENTRY3(80);
```

Tambien en `isr.h` hay que agregar: 

```h
void _isr80();
```

Tambien agrego una variable global dentro del archivo `isr.asm` que se va a llamar `hay_que_modificar_accesed_tarea_entrante` que va a arrancar en 0. 

Por lo que en `isr.asm` agrego: 

```asm
section .data
global hay_que_modificar_accesed_tarea_entrante  
hay_que_modificar_accesed_tarea_entrante db 0x00
```

2) La rutina de atencion de la syscall va a ser: 

```asm
global _isr80

_isr80:
    pushad 
    mov [hay_que_modificar_accesed_tarea_entrante], 1
    popad
    iret
```

3) La nueva rutina de atencion del clock va a ser: 

```asm
;; Rutina de atención del RELOJ
;; -------------------------------------------------------------------------- ;;
global _isr32

_isr32:
    pushad
    ; 1. Le decimos al PIC que vamos a atender la interrupción
    call pic_finish1
    call next_clock
    ; 2. Realizamos el cambio de tareas en caso de ser necesario
    call sched_next_task
    cmp [hay_que_modificar_accesed_tarea_entrante], 0
    je .procedimiento_normal
    ; Si estamos aca es porque hay que modificar las estructuras de paginacion de la tarea a ejecutar
    push ax
    call modificar_accesed
    add esp, 2
    mov [hay_que_modificar_accesed_tarea_entrante], 0
    .procedimiento_normal:
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
En `tss.c` importo la gdt y agrego la funcion: 

```c
pd_entry_t* obtener_cr3(uint16_t segsel) {
    uint16_t idx = segsel >> 3;
    tss_t* tss_pointer = (tss_t*)((gdt[idx].base_15_0) | (gdt[idx].base_23_16 << 16) | (gdt[idx].base_31_24 << 24));
    return tss_pointer.cr3;
}
```

En `mmu.c` importamos agregamos la funcion: 

```c
void modificar_accesed(uint16_t segsel_a_modificar){
  uint32_t cr3 = obtener_cr3(segsel_a_modificar);
  pd_entry_t* page_directory = (pd_entry_t*)CR3_TO_PAGE_DIR(cr3);
  for(int32_t i = 0; i < 1024; i++){
    uint32_t pde_es_user = page_directory[i].attrs & MMU_U;
    if(pde_es_user){
        // Si la pde es supervisor el combinado va a dar supervisor, asi que no toco esas 
        pt_entry_t* page_table_pointer = (pt_entry_t*)MMU_ENTRY_PADDR(page_directory[i].pt);
        for(int32_t j = 0; j < 1024; j++){
            uint32_t pte_es_user = page_table_pointer[j].attrs & MMU_U;
            if(pte_es_user){
                page_table_pointer[j].attrs = page_table_pointer[j].attrs & 0xFDF;
            }
        }
    }
    
  }
  tlbflush(); // Como lo cambiamos quedo desactualizada asi que borramos lo viejo
}
```

La consigna no especifica si hay que hacerlo tanto para la pde como para la pte, ambas o el combinado, asi que lo hago solo para las pte con el resultado combinado.  
