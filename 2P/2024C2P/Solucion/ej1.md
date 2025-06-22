a)
```c
    void buffer_dma(pd_entry_t* pd) {
        mmu_map_page((uint32_t)pd, 0xBABAB000, 0xF151C000, MMU_U |MMU_P);
    }
```

No pongo el atributo R/W ya que me piden que sólo pueda escribir el lector de cartuchos.

b)
```c
    buffer_copy(pd_entry_t* pd, paddr_t phys) {
        copy_page(phys, 0xF151C00); // copia el buffer a la dirección dada por parámetro desde la dir. física del buffer de video
        mmu_map_page((uint32_t)pd, 0xBABAB000, 0xF151C000, MMU_U | MMU_P); // mapea la dir. física a una virtual
    }
```