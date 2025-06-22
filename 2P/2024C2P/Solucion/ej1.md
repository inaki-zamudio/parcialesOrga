a) Definimos en `defines.h`:
```h
#define DMA_VADDR 0xBABAB000
#define BUFFER_PHY 0xF151C000
```
Ponemos esos defines como extern en `mmu.c` y luego definimos la función de la siguiente manera:

```c
void buffer_dma(pd_entry_t* pd){
    mmu_map_page((uint32_t) pd, DMA_VADRR, DMA_PHY, MMU_P | MMU_U);
}
```

b) 

```c
void buffer_copy(pd_entry_t* pd, paddr_t phys){
    copy_page(phys, BUFFER_PHY); // copiamos la página desde BUFFER_PHY a la dirección pasada como parámetro
    mmu_map_page((uint32_t) pd, DMA_VADRR, phys, MMU_P | MMU_U);// realizamos el mapeo correspondiente para que la dirección pueda ser accedida por la tarea
}
```