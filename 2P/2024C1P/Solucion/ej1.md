- Varias tareas en ejecución
- Syscall: espiar memoria de otra tarea.
- Parámetros:
    * Selector a espiar
    * vaddr espiada
    * vaddr espía
- Si la dire no está mapeada en el espacio de direcciones de la espía, return 1. Si no, 0 (en EAX).

a) 

En primer lugar, en `idt.c`:

```c
void idt_init() {
    ...
    IDT_ENTRY3(80);
}
```

Luego, en `isr.asm`:


```nasm
...
; PushAD Order
%define offset_EAX 28
%define offset_ECX 24
%define offset_EDX 20
%define offset_EBX 16
%define offset_ESP 12
%define offset_EBP 8
%define offset_ESI 4
%define offset_EDI 0

; Suponemos que nos llegan los parámetros en los siguientes registros:
EAX -> selector a espiar
ESI -> vaddr espiada
EDI -> vaddr espía

global _isr80
_isr80:
    pushad

    push ESI
    push EDI
    push EAX

    call espiar ; espiar(selector, espiada, espía)

    add esp, 12

    mov [ESP + OFFSET_EAX], eax ; para no pisarlo al popad'ear

    popad
    iret
```

En ``sched.c`
```c
uint32_t espiar(uint16_t segsel, uint32_t* espiada, uint32_t* espía) {
    uint32_t cr3_espiada = obtenerCR3(segsel);

    paddr_t paddr_espiada = obtenerDireccionFisica(cr3_espiada, espiada);

    if (paddr_espiada == 0) return 1;

    mmu_map_page(rcr3(), DST_VIRT_PAGE, paddr_espiada, MMU_P);

    uint32_t dato = *((DST_VIRT_PAGE & 0xFFFFF000) | (espiada & 0xFFF));

    mmu_unmap_page(rcr3(), DST_VIRT_PAGE);

    espía[0] = dato;
}

uint32_t obtenerCR3(uint16_t segsel) {
    return gdt[segsel >> 3].base.cr3
}

paddr_t obtenerDireccionFisica(uint32_t cr3, uint32_t* vaddr) {
    pd_entry_t* pde = CR3_TO_PAGE_DIR(cr3) + VIRT_PAGE_DIR(vaddr);
    if (pde.present == 0) return 0;
    pt_entry_t* pte = (*pde >> 12) + VIRT_PAGE_TABLE(vaddr);
    if (pte.present == 0) return 0;
    paddr_t addr = (*pte >> 12) + VIRT_PAGE_OFFSET(vaddr);
    return MMU_ENTRY_PADDR(addr);
}
```


