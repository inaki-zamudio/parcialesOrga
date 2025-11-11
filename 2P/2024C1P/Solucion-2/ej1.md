idt.c:

```c
void idt_init() {
  ...
  IDT_ENTRY3();
}
```

isr.h:

```h
...
void _isr80();
```

isr.asm:

```nasm
global _isr80
_isr80:
  ; esi -> uint16_t selector_a_espiar
  ; edi -> vaddr_t virt_a_espiar
  ; ecx -> vaddr_t virt_espia
  pushad
  push ESI
  push EDI
  push ECX
  call espiar
  add esp, 12
  popad
  iret
```

```c
uint32_t espiar(uint16_t selector_a_espiar, vaddr_t virt_a_espiar, vaddr_t virt_espia) {
  uint32_t cr3 = obtenerCR3(selector_a_espiar);
  paddr_t paddr_espiada = obtenerDireccionFisica(cr3_espiada, espiada);

  if (!paddr_espiada) return 1;

  mmu_map_page(rcr3(), DST_VIRT_PAGE, paddr_espiada, MMU_P);

  uint32_t dato = *(( DST_VIRT_PAGE & 0xFFFFF000) | (espiada & 0xFFF));

  mmu_unmap_page(rcr3(), DST_VIRT_PAGE);

  espia[0] = dato;
}
```
```
