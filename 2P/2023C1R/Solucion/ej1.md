a) syscall exit: llamada por una tarea, la desactiva y pone a correr la siguiente.
b) modificar a) para que además guarde el ID de quien la llamó en el EAX de la próxima tarea
c) y si ahora la interrupción del reloj es quien modificar EAX lvl 3 de la próxima tarea?
d) es buena práctica que las tareas se comuniquen modificándose los EAX? qué problemas pueden surgir? de qué otra forma se podrían comunicar?

a) 
```c
void idt_entry() {
    ...
    IDT_ENTRY3(80);
}
```
isr.h:
```h
...
void _isr80();
#endif
```

```nasm
global _isr80
_isr80:
    pushad

    call desactivar_current

    call sched_next_task
    
    str bx
    cmp ax, bx
    je .fin

    mov word [sched_task_selector], ax
    jmp far [sched_task_offset]

    .fin:
    popad
    iret
```

```c
void desactivar_current() {
    sched_disable_task(current_task);
}
```

b)

```nasm
global _isr80
_isr80:
    pushad

    call desactivar_current

    call sched_next_task
    str bx
    cmp ax, bx
    je .fin

    push EAX ; segsel de la próxima tarea
    call set_EAX

    mov word [sched_task_selector], ax
    jmp far [sched_task_offset]

    .fin:
    popad
    iret
```

```c
void set_EAX(uint16_t next_task) {
    // modifica el EAX de la tss de la próxima tarea a ejecutar. Esto funciona porque lo hacemos antes del context switch entonces cuando este mismo se haga, va a cargarse el EAX modificado

    obtenerTSS(next_task)->eax = current_task;
}
```