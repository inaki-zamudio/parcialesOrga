A) No voy a dibujar el esquema pero la idea seria que del lado de la memoria virtual debajo del area de la memoria compartida haya un mb de memoria para datos. Va a ser nivel 3 read write. Va a empezar en 0x08004000 y terminar en 0x08104000. Son 256 paginas de 4 kib. Esa memoria virtual va a estar mapeada para cada tarea en un mb distinto entre la direccion fisica 0x400000 y 0x2FFFFFF. 

Para que se produzca este mapeo bien lo que habria que hacer es dentro de `tasks.c` agregar un array similar a `task_code_start` donde se guarde para cada tipo de tarea donde es el inicio de la memoria fisica donde esta su mb de datos. Y a la funcion `tss_create_user_task` agregarle el parametro que indique donde empieza su memoria para datos, y a `mmu_init_task_dir` tambien. Y dentro de `mmu_init_task_dir` agregar un ciclo que mapee las 256 paginas de datos a partir de la direccion virtual 0x08004000. 

B) Para implementar el servicio hay que implementar una syscall. 

Para agregar la nueva syscall vamos a definir una interrupcion nueva en idt_init().

Como las syscalls suelen definirse a partir del numero de interrupcion 80 vamos a definir la syscall como la numero 80.

Para que pueda ser llamada desde la tarea va a ser una IDT_ENTRY3.

Asi que en la funcion `idt_init` agrego: `IDT_ENTRY3(80);`

Tambien en isr.h hay que agregar:

```h
void _isr80();
```

Definimos entonces la rutina de atención de la syscall en `isr.asm`:

```asm
global _isr80
_isr80:
    pushad
    cmp [current_task], 1 
    jne .fin ; Si el id de la tarea no es la 1 no tiene permitido robar
    ; Si estamos aca es porque el que hizo la syscall es la tarea 1 que puede robar
    ; Como nunca llamamos a ninguna funcion de C tanto el registro edi como esi tienen los valores que necesitamos, por lo que no hace falta irlos a buscar a la pila.
    push edi
    push esi
    call copiar_pagina
    add esp, 8
    .fin:
    popad
    iret
```

Agregamos la variable `current_task` de `sched.c` como una variable dentro de `isr.asm` poniendo `extern current_task` y habria que modificar el makefile tambien. 

El lugar fisico donde va a estar mapeada la pagina que roba la tarea con el id 1 va a ser definida cuando se cree la tarea, la variable se va a llamar `PADDR_DONDE_PEGAR`, deberia estar definida dentro del area de datos de la tarea que definimos. 

Ahora defino la funcion `copiar_pagina` en `sched.c`:

```c
void copiar_pagina(vaddr_t direccion_a_copiar, uint32_t id_tarea){
  paddr_t cr3_tarea_a_copiar = obtener_cr3(sched_tasks[id_tarea].selector);

  paddr_t paddr_a_copiar = obtener_paddr(cr3_tarea_a_copiar, direccion_a_copiar);

  copy_page(PADDR_DONDE_PEGAR, paddr_a_copiar); // Otra opcion seria que se pegue en un mmu_next_free_user_page pero me parece que tiene mas sentido que siempre sea en el mismo lugar ya definido de antemano

  // Como copy_page desmapea los datos volvemos a mapear la direccion
  mmu_map_page(rcr3(), direccion_a_copiar, PADDR_DONDE_PEGAR, MMU_P | MMU_U | MMU_W);
}
```

La funcion auxiliar `obtener_cr3` en `sched.c` importando la gdt en ese archivo: 

```c
pd_entry_t* obtener_cr3(uint16_t segsel) {
    uint16_t idx = segsel >> 3;
    tss_t* tss_pointer = (tss_t*)((gdt[idx].base_15_0) | (gdt[idx].base_23_16 << 16) | (gdt[idx].base_31_24 << 24));
    return tss_pointer.cr3;
}
```

Y la funcion `obtener_paddr` en `mmu.c`: 

```c
paddr_t obtener_paddr(uint32_t cr3, vaddr_t virt){
  uint32_t directory_index = VIRT_PAGE_DIR(virt); 
  vaddr_t table_index = VIRT_PAGE_TABLE(virt);

  pd_entry_t* page_directory = (pd_entry_t*)CR3_TO_PAGE_DIR(cr3);
  
  pt_entry_t* page_table_pointer = (pt_entry_t*)MMU_ENTRY_PADDR(page_directory[directory_index].pt);
  pt_entry_t pte_entry = page_table_pointer[table_index];

  paddr_t phy = (paddr_t)MMU_ENTRY_PADDR(pte_entry.page);
  return phy;
}
```
