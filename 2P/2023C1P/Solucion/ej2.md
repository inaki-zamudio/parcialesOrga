La idea va a ser recorrer toda la estructura de paginacion del cr3 que me pasan, y por cada page table entry checkear si esta apuntando al principio de la pagina de la direccion fisica que nos pasan y si esta el bit dirty, osea que se escribio. 

```c
uint8_t escribir_a_disco(int32_t cr3, paddr_t phy){
  pd_entry_t* page_directory = (pd_entry_t*)CR3_TO_PAGE_DIR(cr3);
  for(int32_t i = 0; i < 1024; i++){
    pt_entry_t* page_table_pointer = (pt_entry_t*)MMU_ENTRY_PADDR(page_directory[i].pt);
     for(int32_t j = 0; j < 1024; j++){
      pt_entry_t pte_entry = page_table_pointer[j];
      paddr_t inicio_pagina = (paddr_t)MMU_ENTRY_PADDR(pte_entry.page);
      uint32_t esta_dirty = pte_entry.attrs & 0x40;
      if(esta_dirty && inicio_pagina <= phy && (inicio_pagina + PAGE_SIZE) >= phy){
        return 0;
      }
     }
  }
  return 1;
}
```
