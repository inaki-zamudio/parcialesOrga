```c
vaddr_t* paginas_modificadas(int32_t cr3) {
    uint32_t size = cant_paginas_modificadas(cr3);
    vaddr_t arr[size];
    uint32_t ult_indice = 0;
    pd_entry_t* pd = CR3_TO_PAGE_DIR(cr3);
    for (uint32_t i = 0; i < 1024; i++) {
        pt_entry_t* pt = MMU_ENTRY_PADDR(pd[i]);
        for (uint32_t j = 0; j < 1024; j++) {
            paddr_t pte = pt[j];
            // nos interesa ahora ver el bit Dirty de la pte, ya que, si es 1, indica que se modificó la página referenciada por la pte
            if (pte.d){ // abuso de notación
                // si estamos acá es que la página fue modificada, entonces vamos a reconstruir la dirección virtual que apunta a la base de esa página. 
                // esa dirección virtual se compone de:
                // i (10 bits) + j (10 bits) + offset_3 = 12 bits en 0, pues es la base de una página
                arr[ult_indice] = (i << 22| j << 12); 
                ult_indice++; // actualizo índice del próximo elemento a agregar
            } 
        }
    }
    return arr;
}
```
```c
uint32_t cant_paginas_modificadas(uint32_t cr3) {
    uint32_t cant = 0;
    pd_entry_t* pd = CR3_TO_PAGE_DIR(cr3);

    for (uint32_t i = 0; i < 1024; i++) {
        pt_entry_t* pt = MMU_ENTRY_PADDR(pd[i]);
        for (uint32_t j = 0; j < 1024; j++) {
            paddr_t* pte = pt[j];
            if (pte.d){ // podríamos hacer una máscara que deje al bit 6 nomás (& con 0x40)
                cant++;
            } 
        }
    }
}
```

> Nota: Mati en el resuelto se fija si también está Accessed (eso pasa si se accede a memoria controlada por la pte) pero es necesario eso? ya que no se modifica la página. Tengo entendido que eso sólo pasa cuando se escribe en la página. 

> Otras cosas a tener en cuenta:    
> Importante fijarse si la pde y la pte están presente o no. solo es válido lo que hacemos si están presentes 