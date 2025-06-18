A) Cuando una tarea no privilegiada ejecute HLT se va a producir una general protection fault porque se trato de ejecutar una instruccion privilegiada mientras el cpl no es cero. 

B) Para determinar si la instruccion que se trato de ejecutar fue halt lo que tenemos que hacer es leer si la instruccion a la que apunta el eip de la pila es igual al opcode de HLT, que es 0xF4.

C) Para finalizar el proceso lo que se tiene que hacer es llamar a una funcion en `sched.c` que pause la tarea que se estaba corriendo actualmente que fue la que hizo el halt. 

D) Para determinar el proximo proceso a ejecutar podemos hacer lo mismo que en la interrupcion de clock, pedir el selector de la proxima tarea a ejecutar y hacer un far jump a ella. 

F) El codigo de atencion de la interrupcion va a ser: 

```asm
global _isr13

_isr13:
	; Estamos en un page fault.
	pushad 
    mov eax, [esp + 4] ; Ponemos en eax el instruction pointer que apunta a la instruccion
    mov al, [eax] ; Hacemos que al tenga el opcode de la instruccion a la que apuntaba
    cmp al, 0xF4
    jne .fin
    ; Si estamos aca es porque se llamo a halt
    call disable_current_task
    call sched_next_task
    cmp ax, 0
    je .fin

    str bx
    cmp ax, bx
    je .fin

    mov word [sched_task_selector], ax
    jmp far [sched_task_offset] 
    .fin: 
        popad
        add esp, 4 ; error code 
        iret
```

Y el codigo de `disable_current_task` dentro de `sched.c` va a ser: 

```c
void disable_current_task(){
    sched_disable_task(current_task);
}
```
