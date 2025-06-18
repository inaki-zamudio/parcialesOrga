Asumo que lo que hay que devolver en el array son las direcciones virtuales sin offset que representan el principio de la pagina. 


```c
vaddr_t* paginas_modificadas(int32_t cr3){
  // Primero defino una variable para despues crear el array que va a tener el tamaño del array
  uint32_t arr_size = cantidad_paginas_modificadas(cr3);
  vaddr_t res[arr_size];

  int elemento_actual = 0;

  pd_entry_t* page_directory = (pd_entry_t*) CR3_TO_PAGE_DIR(cr3);
  for(uint32_t i = 0; i < 1024; i++){
    pt_entry_t* page_table_pointer = (pt_entry_t*)MMU_ENTRY_PADDR(page_directory[i].pt);
    for(uint32_t j = 0; j < 1024; j++){
      pt_entry_t pte_entry = page_table_pointer[j];
      uint32_t esta_dirty_y_accesed = pte_entry.attrs & 0x60;
      if(esta_dirty_y_accesed){
        res[elemento_actual] = (i << 22) || (j << 12);
        elemento_actual = elemento_actual + 1;
      }
    }
  }
  return res;
}

uint32_t cantidad_paginas_modificadas(int32_t cr3){
  uint32_t res = 0;
  pd_entry_t* page_directory = (pd_entry_t*) CR3_TO_PAGE_DIR(cr3);
  for(uint32_t i = 0; i < 1024; i++){
    pt_entry_t* page_table_pointer = (pt_entry_t*)MMU_ENTRY_PADDR(page_directory[i].pt);
    for(uint32_t j = 0; j < 1024; j++){
      pt_entry_t pte_entry = page_table_pointer[j];
      uint32_t esta_dirty_y_accesed = pte_entry.attrs & 0x60;
      if(esta_dirty_y_accesed){
        res = res + 1;
      }
    }
  }
  return res;
}
```
