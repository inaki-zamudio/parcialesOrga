## Características del sistema pedido:
- 5 tareas
- El sistema actualizará el ECX de todas las tareas, incrementando su valor cada vez que la tarea vuelva a ser ejecutada luego de una interrupción del reloj
- Servicio _fuiLlamadaMasVeces_ que permite que una tarea compare su valor de ecx con alguna otra. Se pasa por EDI el id de la otra tarea (van del 0 al 4), y devuelve el resultado en EAX. Da 0 si la llamadora tiene UTC <= que la ota tarea, y 1 en caso contrario.

a)

En la GDT, se agregarán los descriptores de segmento de las 5 TSS correspondientes a las 5 tareas del sistema.

b)

Cada vez que se salte a una tarea luego de una interrupción de reloj, habrá que acceder a su TSS para poder leer el ESP, y modificar el valor del registro ECX de la pila.

Entonces, habría que modificar la interrupción del reloj para que haga una llamada a una función de C que modifique el ecx de la tarea que se está ejecutando.

```nasm
...
extern incrementarECX
...
global _isr32

_isr32:
    pushad
    ; 1. Le decimos al PIC que vamos a atender la interrupción
    call pic_finish1
    call next_clock
    
    str ax ; guardo en EAX el segsel de la tarea actualmente en ejecución
    push ax
    call incrementarECX
    pop ax
 
    ; 2. Realizamos el cambio de tareas en caso de ser necesario
    call sched_next_task

    cmp ax, 0
    je .fin

    str bx
    cmp ax, bx
    je .fin

    mov word [sched_task_selector], ax

    jmp far [sched_task_offset]

    .fin:
    ; 3. Actualizamos las estructuras compartidas ante el tick del reloj
    call tasks_tick


    ; 4. Actualizamos la "interfaz" del sistema en pantalla
    call tasks_screen_update


    popad

    iret
```

```c
void incrementarECX(uint16_t segsel) {
    tss_t* tss = obtenerTSS(segsel);
    tss->esp[6]++; // incremento ECX
}
```

c)

Habría que agregar una nueva syscall para que pueda ser llamada por las tareas. Para eso, hay que agregar la declaración de la interrupción en isr.h, y luego programar el handler en isr.asm. También es necesario agregar, en la función idt_init() de idt.c, la declaración con la macro IDT_ENTRY3(80).

En isr.h:

```h
...
void _isr80();
```

En idt.c:

```c
void idt_init() {
    ...
    IDT_ENTRY3(80);
```

d)

En isr.asm:

```nasm
...
extern fuiLlamadaMasVeces
...
global _isr80
_isr80:
    ; edi -> int8_t task_id
    pushad
    push edi
    push ecx
    call fuiLlamadaMasveces
    pop ecx
    pop edi 
    popad
    iret
```

```c
uint8_t fuiLlamadaMasVeces(uint32_t task_id, uint32_t utc_propio) {
    // compara el utc de la tarea actualmente en ejecución
    // con el utc de la tarea que me pasan por parámetro
    uint16_t segsel = sched_tasks[task_id].selector;
    tss_t* tss = obtenerTSS(segsel);
    uint32_t utc = tss->esp[ecx];
    
    return utc_propio > utc;
}

tss_t* obtenerTSS(uint16_t selector){
    uint16_t idx = selector >> 3;
    return gdt[idx].base // en realidad acá es un abuso de anotación, ya que se guarda en partes en la GDT entry
}
```
