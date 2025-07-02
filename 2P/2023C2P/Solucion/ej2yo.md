a) Un GP fault, ya que el CPL del segsel no es 0.
b) (Mati) Hay que ver que el opcode de la tarea sea el mismo que HLT, y dice que es 0xF4 (por qué es esto?)
c) Basta con pausar la tarea y ya, no?
d) Lo que hace la isr32
e) Habría que hacer lo de (b):

```nasm
global _isr14

_isr14:
	; Estamos en un page fault.
	pushad
  mov ecx, cr2
  push ecx
  call page_fault_handler
  pop ecx
  cmp al, 1
  jmp .fin
  .ring0_exception:
; Si llegamos hasta aca es que cometimos un page fault fuera del area compartida.
  call kernel_exception
  jmp $

.fin:
	popad
	add esp, 4 ; error code
	iret
```
```
