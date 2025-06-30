a)
Primero, habría que definir en `idt.c`:
```c
void idt_init() {
	...
	IDT_ENTRY0(40);
}
```

Luego, en `isr.asm`:
```nasm
...
global _isr40
_isr80:
	pushad
	
	; le decimos al PIC que vamos a atender la interrupción
	call pic_finish1
	call deviceready
	
	popad
	iret
```

En `sched.c` modificamos el struct `sched_entry_t`:

```c
typedef struct {
	int16_t selector;
	task_state_t state;
	int8_t pausada_por_open; // opendevice() la setea en 1, seteada en 0 por closedevice() y en _isr40
	int8_t acceso; // 0, 1, o 2
} sched_entry_t;
```

Y agregamos la función `deviceready`:
```c
void deviceready() {
	for (uint8_t i = 0 i < MAX_TASKS; i++) {
		pd_entry_t* pd = obtCR3(sched_tasks[i].selector);
		if (sched_tasks[i].pausada_por_open) {
			if (sched_tasks[i].acceso == 1) {
				buffer_dma(pd);
			}
			sched_tasks[i].pausada_por_open = 0;
			sched_tasks[i].state = TASK_RUNNABLE;
		}
		if (sched_tasks[i].acceso == 2) {
			buffer_copy(pd, sched_tasks[i].paddr_copia, sched_tasks[i].vaddr_copia);
		}
	}
}
```

En `tss.c` agregamos la función:
```c
pd_entry_t* obtCR3(uint16_t segsel) {
	uint16_t idx = segsel.indice;
	tss_t* tss_pointer = (tss_t*) (gdt[idx].base);
	return tss_pointer.cr3;
}

b)
```c
void idt_init() {
	...
	IDT_ENTRY3(80);
	IDT_ENTRY3(81);
}
```
En `isr.asm`:
```
...
global _isr80
_isr80:
	pushad
	
	mov al [0xACCE50] ; veo el uint8_t acceso. Lo puedo acceder directamente xq el mapa de memoria es el mismo que la tarea
	cmp al, 0
	je .acceso_en_0

	cmp al, 1
	je .acceso_dma

	; acceso_copia
	str ax
	push ax
	call obtener_ecx	

	popad
	iret
