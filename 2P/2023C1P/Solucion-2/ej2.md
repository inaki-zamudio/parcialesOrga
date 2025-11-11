Para determinar si la tarea específica escribió la página que se está desalojando, se puede, por medio del cr3, acceder al directorio de páginas de la tarea. Allí, se buscará la PDE correspondiente fijándose en si la traducción hasta la dirección física de la página coincide con la dirección física que se pasa por parámetro. De ser así, se fija en la PDE si está seteado el bit D (dirty).

```c
uint8_t Escribir_a_disco(int32_t cr3, paddr_t phy) {
    pd_entry_t* pd = CR3_TO_PAGE_DIR(cr3);
    for (uint32_t i = 0; i < 1024; i++) { // recorrer toda la PD
        pd_entry_t pde = pd[i];
        if (pde.attrs & MMU_P) { // si la traducción no es válida, no la miro
            pt_entry_t* pt = pde & 0xFFFFF000; // 20 bits más altos 
            for (uint32_t j = 0; j < 1024; j++) {
                paddr_t pte = pt[j];
                if (MMU_ENTRY_PADDR(pte) == phy) {
                    if (pte.attrs & MMU_DIRTY) {
                        return 0;
                    }    
                }
            }
        }
    } 
    return 1;
}
```
