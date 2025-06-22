A) En `defines.h` agregamos: 
```h
#define BUFFER_PADDR 0xF151C000
#define DMA_VADDR 0xBABAB000
```

Y en `mmu.c` agregamos esas constantes como externs. 

```C
void buffer_dma(pd_entry_t* pd){
  mmu_map_page(pd, DMA_VADDR, BUFFER_PADDR, MMU_U | MMU_P);
}
```
B) Había que agregar que la función tambien recibe la dirección virtual a la cual se le hace la copia. 

`copy_page` primero mapea ambas direcciones fisicas a direcciones virtuales dentro del directorio de la tarea actual, copia el dato de src a destino y luego desmapea las dos direcciones virtuales. Como el dato queda pegado en la direccion fisica src pero sin mapear entonces despues hay que mapearlo a la direccion virtual que nos pasan.

Voy a primero usar `copy_page` para que se copie el contenido de `BUFFER_PADDR` a la dirección __fisica__ pasada por parametro. Como `copy_page` mapea a `SRC_VIRT_PAGE` y despues lo desmapea no tenemos que preocuparnos por desmapear esa dirección. Pero si tenemos que mapear la dirección fisica a la que se le copio la pagina a la dirección virtual que nos pasan. 

```c
void buffer_copy(pd_entry_t* pd, paddr_t phys, vaddr_t vaddr){
    copy_page(phys, BUFFER_PADDR);
    mmu_map_page(pd, vaddr, phys, MMU_U | MMU_P);
}
```