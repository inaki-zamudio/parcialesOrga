a)
> Características del servicio: 
> - 5 tareas nivel 3 (1)
> - 1 tarea nivel 0 (2)
> - (1) escriben en EAX resultados y (2) los puede leer
> - (1) le ceden tiempo de ejecución a (2) y se quedan pausadas

Agregamos la entrada correspondiente a la syscall en la IDT:

```c
void idt_init(){
    ...
    IDT_ENTRY3(80);
}
```

Y la declaramos en `idt.h`. 

b)

dudas:
- por qué es necesario habilitar y deshabilitar la tarea 6?
- cuándo sabemos que la tarea 6 terminó de utilizar el resultado en eax? Se encarga de usar el resultado y luego vuelve a activar a la tarea que llamó a la syscall

---

```nasm

sched_task_selector: dw 0xFAFA
sched_task_offset: dd 0
...

global _isr80
_isr80:
    pushad

    ; deshabilita tarea actual, habilita tarea 6, pone el eax en la tss de la tarea 6
    push eax ; le pasamos el resultado del cálculo
    call modificarEAX
    add esp, 4

    ; context switch = cede tiempo de ejecución a tarea 6
    mov word [sched_task_selector], ax
    jmp far [sched_task_offset]

    popad
    iret
```

Luego definimos la función `XXXXX`:

```c
uint16_t modificarEAX(uint32_t res){
    sched_disable_task(current_task); // pausa la tarea actual
    sched_enable_task(task6_id); // task6_id será una variable global

    // como la tarea 6 recibirá en eax res, tenemos que ponerlo en ese registro en su tss

    tss_t* tarea6_tss = &(tss_tasks[task6_id]); // puntero a la tss de la tarea 6
    tarea6_tss.eax = res; // actualizamos el valor del eax
    tarea_desalojada = current_task; // var global

    return sched_tasks[task6_id].selector; // retornamos el selector de la tarea 6 para que cuando se haga el context switch salte ahí
}
```
c) 

```c
while(true){
    uint32_t eax = tss_tasks[current_task].eax; // obtengo eax de la tss de la tarea 6: es legal? creo que no, pero entonces: cómo accedo a la pila para ver el eax más reciente??
    procesar(eax); // función que hace cualquier cosa
    
    sched_enable_task(tarea_desalojada); // despausa la tarea
    sched_disable_task(current_task);
    cambiar_tarea();
}
```

```nasm
global cambiar_tarea
cambiar_tarea:
    pushad

    call sched_next_task
    
    mov word [sched_task_selector], ax
    jmp far [sched_task_offset]

    popad
    iret
```