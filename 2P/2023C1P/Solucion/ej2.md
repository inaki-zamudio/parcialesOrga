```c
uint8_t escribir_a_disco(uint32_t cr3, paddr_t phy) {
    for (uint32_t i = 0; i < 1024; i++) {
        pd_entry_t* pd = CR3_TO_PAGE_DIR(cr3);
        pd_entry_t pde = pd[i];
        if (pde.attrs & MMU_P) {
            for (uint32_t j = 0; j < 1024; j++) {
                pt_entry_t* pt = CR3_TO_PAGE_DIR(pde); // lo usamos para agarrar los 20 bits más altos
                paddr_t* pte = pt[j];
                if (pte.attrs & MMU_P) {
                    if (pte.attrs & MMU_DIRTY) {
                        if (MMU_ENTRY_PADDR(pte) == phy) {
                            return 0;
                        }
                    }
                }
            }
        }
    }
    return 1;
}
```