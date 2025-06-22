a) 
Declaramos la rutina en `isr.h`:
```h
...
void _isr40();
```
En el archivo `idt.c` agregamos en la función `idt_init()`:
```c
void idt_init(){
    ...
    IDT_ENTRY0(40);
}
```
Ya que se trata de una interrupción externa (nivel kernel).

Definimos `_isr40` de la siguiente forma:

```nasm
extern deviceready
...
global _isr40
_isr40:
    pushad
    call pic_finish1 ; llamamos a pic_finish1 ya que se trata de una interrupción externa
    call deviceready ; función principal que se encarga de que las tareas que lo requieran puedan acceder al buffer
    popad
    iret
```

Declaramos `deviceready` en `sched.h` como
```h
...
void deviceready();
```
Ahora definiremos `deviceready` en `sched.c` de la siguiente manera:

```c
void deviceready(){
    vaddr_t acceso_dir = 0xACCE50;
    for (int i = 0; i < MAX_TASKS; i++){ // iteramos por todas las tareas
        sched_entry_t task = sched_tasks[i];
        if (task.state == TASK_RUNNABLE){ // si la tarea no existe o está pausada, no hacemos nada
            int16_t selector = task.selector;
            uint32_t cr3 = obtener_cr3(selector); // obtenemos la base del page directory en base al selector de la tarea
            paddr_t phy_addr = phy_addr(cr3, acceso_dir); // traducción virt -> phy
            uint8_t acceso = *((uint8_t*) phy_addr);
            if (acceso == 1){
                buffer_dma((pd_entry_t*) cr3);
            } else if (acceso == 2){
                buffer_copy((pd_entry_t*) cr3, phy_addr);
            }
        }
    }
}

uint32_t obtener_cr3(int16_t task_selector){
    uint16_t idx = task_selector >> 3;
    gdt_entry_t tss_descriptor = gdt[idx];
    int32_t tss_base_addr = tss_descriptor.base_15_0 | tss_descriptor.base_23_16 << 16| tss_descriptor.base_31_24 << 24;
    return (uint32_t) tss_base_addr.cr3;
}

paddr_t phy_addr(uint32_t cr3, vaddr_t virt){
    uint32_t pd_base_addr = cr3 & 0xFFFFF000;
    uint32_t dir_offset = virt >> 22;
    uint32_t table_offset = virt >> 12;
    uint32_t page_offset = virt & 0xFFF;
    pd_entry_t pde = ((pd_entry_t*) pd_base_addr)[dir_offset];
    pt_entry_t pte = ((pt_entry_t*) pde)[table_offset];
    paddr_t phy_addr = ((paddr_t*) pte)[page_offset];
    return phy_addr;
}
```

- El selector de segmento de la tarea (sched_tasks[i]) es el selector del Task Register.
- En base al selector conseguimos el cr3 (buscamos en la GDT el TSS Descriptor que tiene la base del TSS, luego el TSS tiene, entre otros, el valor de cr3).
- Como el cr3 nos indica la base del page directory, con la dirección virtual (0xACCE50) obtenemos el offset para encontrar el pde correspondiente a esa dirección y hacemos todos los pasitos para conseguir la dirección física.

b) 
`opendevice` será la interrupción 90 y `closedevice` será la interrupción 91.

Declaramos en `idt.h`:
```h
...
void _isr90();
void _isr91();
```

Y en `isr.c`:
```c
void idt_init(){
    ...
    IDT_ENTRY3(90);
    IDT_ENTRY3(91);
}
```

```nasm
global _isr90
_isr90:
    pushad
    
    popad
    iret

global _isr91
_isr91:
    pushad

    popad
    iret
```

> Dudas: 
> 1. Está bien el a)? específicamente el deviceready.
> 2. Cómo se hace el b)?      
>    - Cómo sabemos cuándo el buffer está listo?