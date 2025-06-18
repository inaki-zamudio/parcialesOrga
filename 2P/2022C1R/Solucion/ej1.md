1) Para definir la syscall: 

Para agregar la nueva syscall vamos a definir una interrupcion nueva en idt_init().

Como las syscalls suelen definirse a partir del numero de interrupcion 80 vamos a definir la syscall como la numero 80.

Para que pueda ser llamada desde las tareas va a ser una IDT_ENTRY3.

Asi que en la funcion `idt_init` agrego: `IDT_ENTRY3(80);`

Tambien en isr.h hay que agregar:

```h
void _isr80();
```

2) El codigo de la syscall va a ser: 

```asm
global _isr80
_isr80:
    pushad
    call copiar_tarea_actual
    popad
    iret
```