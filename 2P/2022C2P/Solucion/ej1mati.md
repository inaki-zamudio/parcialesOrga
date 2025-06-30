A) Para que el servicio pueda ser invocado hay que definir la syscall en la idt. Para eso En la funcion `idt_init` agregamos `IDT_ENTRY3(100)` para definir la interrupcion en la idt. Como queremos que sea llamada desde las tareas va a ser de nivel 3. 

Luego en `isr.h` agregamos `void _isr100();`. 

La convencion que voy a usar para recibir los parametros desde la syscall va a ser que en `eax` esta la direccion virtual, en `edx` la direccion fisica y en `di` el selector de segmento que apunta al descriptor de tss en la gdt. 

B) Para que la tarea actual retome su ejecucion en la direccion pasada lo que voy a hacer es modificar el eip que se guarda en la pila de la interrupcion para que apunte a donde queremos que siga el codigo, para que cuando salgamos de la interrupcion cuando se le ponga el valor del iret al esi sea el valor modificado. 

Y para la tarea que nos llama con su selector vamos a acceder a su tss y modificar el eip de la pila de la tss. 

Pero antes de eso vamos a mapear la pagina virtual y fisica en el directorio actual y en el de la tarea que nos llama. 

La rutina de atencion va a ser: 

```asm
global _isr32
_isr32:
    pushad
    mov [esp + 0x20], eax ; Con esto hago que el eip de la pila apunte a la direccion virtual
    ; Aunque todavia esa direccion virtual no esta mapeada despues la vamos a mapear antes que usarla
    mov edi, [esp + 0x2C] ; Pongo en edi el esp3 de la pila
    and edi, 0xFFFFF000
    add edi, 0x1000
    mov [esp + 0x2c], edi ; Actualizo el esp3 de la pila para reiniciarla cuando salgamos de la interrupcion 
    push eax
    push edx
    push di
    call force_task
    add esp, 6
    pop eax
    popad 
    iret
```

La implementacion de `force_task` va a ser: 

```c
void force_task(uint16_t segsel_otra_tarea, vaddr_t vaddr_a_mapear, paddr_t paddr_a_mapear){
  pd_entry_t* cr3_otra_tarea = obtener_cr3(segsel_otra_tarea);

  mmu_map_page(rcr3(), vaddr_a_mapear, paddr_a_mapear, MMU_P | MMU_U);
  mmu_map_page(cr3_otra_tarea, vaddr_a_mapear, paddr_a_mapear, MMU_P | MMU_U);
  
  uint16_t idx_otra_tarea = segsel_otra_tarea >> 3;
  tss_t* tss_pointer_otra_tarea = (tss_t*)((gdt[idx].base_15_0) | (gdt[idx].base_23_16 << 16) | (gdt[idx].base_31_24 << 24));

  tss_pointer_otra_tarea->eip = vaddr_a_mapear;
  tss_pointer_otra_tarea->cs = GDT_CODE_3_SEL;
  tss_pointer_otra_tarea->ds = GDT_DATA_3_SEL;

  uint32_t* esp3_otra_tarea = tss_pointer_otra_tarea -> esp0 + 44;
  tss_pointer_otra_tarea->esp = (*esp3_otra_tarea & 0xFFFFF000) + 0x1000;

  tss_pointer_otra_tarea->esp0 = (tss_pointer_otra_tarea->esp0 & 0xFFFFF000) + 0x1000;

  // Como la consigna no pide desmapear no desmapeo
}


pd_entry_t* obtener_cr3(uint16_t segsel) {
    uint16_t idx = segsel >> 3;
    tss_t* tss_pointer = (tss_t*)((gdt[idx].base_15_0) | (gdt[idx].base_23_16 << 16) | (gdt[idx].base_31_24 << 24));
    return tss_pointer->cr3;
}

void modificar_eip(uint16_t segsel, uint32_t dato){
  uint16_t idx = segsel >> 3;
  tss_t* tss_task = (tss_t*)((gdt[idx].base_15_0) | (gdt[idx].base_23_16 << 16) | (gdt[idx].base_31_24 << 24));
  tss_task->eip = dato;
}
```
