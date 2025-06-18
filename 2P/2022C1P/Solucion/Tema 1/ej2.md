```c
uint32_t getPhysical(uint32_t virtual, pd_entry_t* pdt, uint32_t *attrs){
  uint32_t directory_index = VIRT_PAGE_DIR(virtual); 
  uint32_t table_index = VIRT_PAGE_TABLE(virtual);
  
  pt_entry_t* page_table_pointer = (pt_entry_t*)MMU_ENTRY_PADDR(pdt[directory_index].pt);
  pt_entry_t pte_entry = page_table_pointer[table_index];

  paddr_t phy = (paddr_t)MMU_ENTRY_PADDR(pte_entry.page); // Shifteo en 12

  uint32_t pde_attributes = pdt[directory_index].attrs;

  uint32_t pte_attributes = pte_entry.attrs;

  // Para calcular el combinado para determinar si es user o supervisor con hacer un and basta
  // Si hay algun supervisor va a quedar supervisor read write

  // Primero me quedo con los ultimos 3 bits de los dos 

  pde_attributes = pde_attributes & 0x7;
  pte_attributes = pte_attributes & 0x7;

  uint32_t privilegio_pde = pde_attributes & 0x4;
  uint32_t privilegio_pte = pte_attributes & 0x4;

  if(privilegio_pde == 1 && privilegio_pte == 1){
    // Si ambos son user con un and basta
    attrs = pde_attributes & pte_attributes;
  }
  else{
    // Con que haya un supervisor va a quedar supervisor read/write
    attrs = MMU_W;
  }
  return phy;
}
```