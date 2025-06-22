Primero definimos en `defines.h` las constantes: 

```h
#define TASK_LOCKABLE_PAGE_VIRT 0x08003000
#define TASK_LOCKABLE_PAGE_PHY 0x0001D000
```

En `sched.c` agregamos la variable global para el archivo `task_with_lock`: 

```c
static int task_with_lock = -1; // Arranca en -1 que representa que ninguna tarea lo tiene, cuando se lo saquemos a la tarea que lo libere tambien lo vamos a poner en -1. Asi sabemos si esta en uso y quien lo tiene en la misma variable. 
```

```c
void get_lock(vaddr_t shared_page){
    if(shared_page == TASK_LOCKABLE_PAGE_VIRT){
        task_with_lock = current_task;
        for (int8_t i = 0; i < MAX_TASKS; i++) {
            if(i != current_task){
                pd_entry_t* cr3 = obtener_cr3(sched_tasks[i].selector);
                mmu_unmap_page((uint32_t)cr3, shared_page);
            }
        }
        // Aca me parece que tendria sentido que se mapee la shared de la tarea, asi que lo voy a hacer por mas que la consigna no lo pida para hacer mas facil la resolucion del segundo punto
        pd_entry_t* cr3_actual = obtener_cr3(sched_tasks[i].selector);
        mmu_map_page((uint32_t) cr3_actual, shared_page, TASK_LOCKABLE_PAGE_PHY, MMU_P | MMU_W | MMU_U);
    }
}
```

Y en `tss.c` exportamos la gdt y agregamos la función:

```c
pd_entry_t* obtener_cr3(uint16_t segsel) {
    uint16_t idx = segsel >> 3;
    tss_t* tss_pointer = (tss_t*)((gdt[idx].base_15_0) | (gdt[idx].base_23_16 << 16) | (gdt[idx].base_31_24 << 24));
    return tss_pointer->cr3;
}
```
