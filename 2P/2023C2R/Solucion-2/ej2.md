a)

> Cada tarea tiene asignado 1MB de memoria para datos. Esas son 256 páginas read-write.

b)

Hay que agregar al systema una syscall. Esta, si es llamada por la tarea cuyo id es 1, accederá a la página mapeada en la memoria virtual de la tarea pasada por parámetro, y copiará sus contenidos en la misma dirección virtual de la tarea 1.

En idt.c:

```c
void idt_init() {
  ...
  IDT_ENTRY3(80);
}
```

En isr.h:

```h
...
void _isr80();
```

Luego, en isr.asm implemento la rutina de atención a la interrupción:

```asm
...
extern CopiarPagina
...
global _isr80
_isr80:
  pushad
  push EDI
  push ESI
  call CopiarPagina
  add ESP, 8
  popad
  iret
```

```c
void CopiarPagina(uint8_t task_id, vaddr_t virt) {
  if (current_task != 1) return; // si la tarea que llamó a la syscall no es la maliciosa, corto
  paddr_t cr3_victima = obtenerCR3(sched_tasks[id_tarea].selector);
  paddr_t phy_victima = obtenerDireccionFisica(cr3_victima, virt); 
  copy_page2(phy_tarea_maliciosa, phy_victima, cr3_victima);
}

uint32_t obtenerCR3() {}

paddr_t obtenerDireccionFisica() {}

void copy_page2(paddr_t dst_addr, paddr_t src_addr, uint32_t cr3_victima) {
  // obtener el cr3
  uint32_t cr3 = rcr3();

  // mapear las páginas
  mmu_map_page(cr3, VIRT_PAGE, dst_addr, (MMU_P | MMU_W | MMU_R));
  mmu_map_page(cr3_victima, VIRT_PAGE, src_addr, (MMU_P | MMU_W));

  // copiar src a dst
  uint32_t* src = (uint32_t*)VIRT_PAGE;
  uint32_t* dst = (uint32_t*)VIRT_PAGE;

  // copiar la página src a dst
  for(uint32_t i=0; i<1024; i++){
    dst[i] = src[i];
  }
}
```
