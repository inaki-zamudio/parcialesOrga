Primero implemento una funcion en `mmu.c` llamada `is_mapped` que dado un cr3 y una direccion virtual va a devolver si fue mapeada o no. 

Para ver si esta mapeado vamos a necesitar que tanto el page directory entry como el page table entry que mapean esa direccion esten present. 

```c
uint8_t is_mapped(uint32_t cr3, vaddr_t virt){
  uint32_t directory_index = VIRT_PAGE_DIR(virt); 
  uint32_t table_index = VIRT_PAGE_TABLE(virt);

  
  pd_entry_t* page_directory = (pd_entry_t*) CR3_TO_PAGE_DIR(cr3);
  pd_entry_t dpt_entry = page_directory[directory_index]; //Convierte la direccion que obtuvimos para poder acceder a .pt y .attr
  uint32_t present_bit_directory = (dpt_entry.attrs) & MMU_P;

  pt_entry_t* page_table_pointer = (pt_entry_t*)MMU_ENTRY_PADDR(page_directory[directory_index].pt) ;
  pt_entry_t pt_entry = page_table_pointer[table_index];
  uint32_t present_bit_table = (pt_entry.attrs) & MMU_P;

  return present_bit_directory && present_bit_table;
}
```

Y ahora defino en `tss.c` la funcion `getMappings`. Para determinar si un elemento de la gdt es un tss descriptor el limite menos la base tiene que tener el tamaño de un elemento `tss_t`, el tipo tiene que ser `DESC_TYPE_32BIT_TSS` el bit `S` tiene que ser 0 y tiene que estar presente. 

Una vez que determinamos que un elemento es un tss descriptor agarramos su cr3 de la tss y llamamos a la funcion anterior. 

Agrego las funciones en `tss.c`:

```c
uint32_t getMappings(uint32_t virtual, gdt_entry_t* gdt){
  uint32_t res = 0;
  for(uint32_t i = 0; i < GDT_COUNT; i++){
    if(es_un_tss_descriptor(gdt[i])){
      res = res + is_mapped(obtener_cr3(gdt[i]), virtual);
    }
  }
  return res;
}

uint8_t es_un_tss_descriptor(gdt_entry_t gdt_entry){
  uint32_t base_segmento = ((gdt_entry.base_15_0) | (gdt_entry.base_23_16 << 16) | (gdt_entry.base_31_24 << 24));
  uint32_t limite_segmento = gdt_entry.limit_15_0 | (gdt_entry.limit_19_16 << 16);
  uint32_t segment_size = limite_segmento - base_segmento;
  return (gdt_entry.type == DESC_TYPE_32BIT_TSS && gdt_entry.s == 0 && segment_size == sizeof(tss_t));
}

uint32_t obtener_cr3(gdt_entry_t gdt_entry){
  // Sabemos que si o si el elemento es un descriptor de tss
  tss_t* tss_task = (tss_t*)((gdt_entry.base_15_0) | (gdt_entry.base_23_16 << 16) | (gdt_entry.base_31_24 << 24));
  return tss_task->cr3;
}
```