Para agregar la syscall nueva vamos a definir la interrupcion nueva en `idt_init()`

Como las syscalls suelen definirse a partir del numero de interrupcion 80 la voy a definir como la 80.

Asi que agregamos en `idt_init()`:

```c
IDT_ENTRY3(80);
```

Para que pueda ser llamada desde la tareas tiene que ser IDT_ENTRY3. Tambien en isr.h hay que agregar:

```h
void _isr80();
```

Para pasar los parametros establezco por convencion mia que la tarea espia va a poner los parametros en:

* En ax el selector de la tarea a espiar. 

* En edi la direccion virtual a leer de la tarea espiada.

* En esi la direccion virtual a escribir de la tarea espia.

El plan va a ser primero obtener la direccion fisica a la cual esta mapeada la direccion virtual de la tarea espiada. Si no esta mapeada devuelvo 0 y terminamos. Para saber si esta mapeada voy a primero obtener el cr3 de la tarea espiada a traves de su tss. 

Si esta mapeada lo que voy a hacer es con la direccion fisica que obtuve mapearla a la direccion virtual de la tarea espia. 

Defino el codigo de la interrupcion: 

```asm
global _isr80
_isr80:
    pushad
    push esi
    push edi
    push ax
    call espiar
    mov [ESP + 28], EAX
    popad 
    iret
```
Y el codigo de la funcion espiar es: 

```c
int8_t espiar(uint16_t segsel_espiada, vaddr_t virtual_espiada, vaddr_t virtual_espia){
  uint32_t cr3_espiada = obtener_cr3(segsel_espiada);

  uint32_t directory_index_espiada = VIRT_PAGE_DIR(virtual_espiada); 
  uint32_t table_index_espiada = VIRT_PAGE_TABLE(virtual_espiada);

  
  pd_entry_t* page_directory_espiada = (pd_entry_t*) CR3_TO_PAGE_DIR(cr3_espiada);
  pd_entry_t dpt_entry_espiada = page_directory_espiada[directory_index_espiada]; //Convierte la direccion que obtuvimos para poder acceder a .pt y .attr
  uint32_t present_bit_directory_espiada = (dpt_entry_espiada.attrs) & MMU_P;

  if(!present_bit_directory_espiada){
    return 1;
  }

  pt_entry_t* page_table_pointer_espiada = (pt_entry_t*)MMU_ENTRY_PADDR(page_directory_espiada[directory_index_espiada].pt) ;
  uint32_t present_bit_table_espiada = (page_table_pointer_espiada[table_index_espiada].attrs) & MMU_P;

  if(!present_bit_table_espiada){
    return 1;
  }
  
  paddr_t fisica_espiada = MMU_ENTRY_PADDR(page_table_pointer_espiada[table_index_espiada].page); // Este dato no tiene offset.

  mmu_map_page(rcr3(), DST_VIRT_PAGE, fisica_espiada, MMU_P | MMU_U);

  uint32_t offset_espiada = (virtual_espiada & 0XFFF);

  uint32_t* puntero_al_dato = (uint32_t*)(DST_VIRT_PAGE || offset_espiada);

  uint32_t* puntero_a_pegar = (uint32_t*)(virtual_espiada);

  puntero_a_pegar = &puntero_al_dato;

  mmu_unmap_page(rcr3(), DST_VIRT_PAGE);

  return 0;
}
```

Y el codigo de `obtener_cr3` es: 

```c
pd_entry_t* obtener_cr3(uint16_t segsel) {
    uint16_t idx = segsel >> 3;
    tss_t* tss_pointer = (tss_t*)((gdt[idx].base_15_0) | (gdt[idx].base_23_16 << 16) | (gdt[idx].base_31_24 << 24));
    return tss_pointer.cr3;
}
```
