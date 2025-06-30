a)
syscall(uint32_t virt,
        uint32_t phy,
        uint16_t task_sel)

idt.c:
```c
void idt_init() {
    ...
    IDT_ENTRY3(100);
}
```

porque nos pide que cualquier tarea pueda llamarla.

isr.h:
```h
...
void _isr100();
#endif
```

b)
La rutina de atención de la interrupción queda:
```nasm
global _isr100
; EDI -> uint32_t virt
; ESI -> uint32_t phy
; DX  -> uint16_t task_sel

_isr100:
    pushad

    push edx
    push esi
    push edi
    push esp ; esp de nivel 0 para poder acceder acceder a dicha pila
    call x
    add esp, 16

    popad
    iret
```

```c
void x(uint32_ esp, uint32_t virt, uint32_t phy, uint16_t task_sel) {
    uint32_t cr3 = rcr3();
    uint32_t cr3_task = obtenerCR3(task_sel);
    mmu_map_page(cr3, virt, phy, MMU_U | MMU_P);
    mmu_map_page(cr3_task, virt, phy, MMU_U | MMU_P);

    // modificamos la TSS de la tarea pasada por parámetro para que siga la ejecución desde la dirección q queremos
    modificarEIP_TSS(task_sel, virt);

    // modificar valor del eip en el stack de la tarea
    modificarEIP_pila(virt, esp);
}

uint32_t obtenerCR3(uint16_t task_sel) {
    return obtenerTSS(task_sel)->cr3;
}

uint32_t obtenerTSS(uint16_t segsel) {
    uint16_t idx = selector >> 3;
    return gdt[idx].base;
}

void modificarEIP_TSS(uint16_t task_sel, uint32_t virt) {
    uint32_t tss = obtenerTSS(task_sel);
    tss->eip = virt;
}

void modificarEIP_pila(uint32_t virt, uint32_t esp) {
    esp[9] = virt; // pila[9] sería el EIP. 
}
```
c) 
```nasm
main:
    ...
    push DX
    push ESI
    push EDI

```

## Conclusiones:
- Qué EIP agarra el scheduler al continuar la ejecución de la tarea?
- El ESP de la tss de la tarea actual no está actualizado, por eso tenemos que pasarlo como parámetro a la función en C
> ⚠️ En la primer versión que hicimos, hacíamos esto:
>```c
>void modificarEIP_TSS(uint16_t task_sel, uint32_t virt) {
>    uint32_t tss = obtenerTSS(task_sel);
>    tss->eip = virt;
>}
> ```
> Esto está incompleto: falta resetear las pilas