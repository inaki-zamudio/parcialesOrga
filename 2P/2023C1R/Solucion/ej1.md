a)

Hay que definir una syscall nueva, que va a pausar la tarea que la llame, y ponga a ejecutar a la siguiente.

Defino una nueva entrada en la IDT:

```c
void idt_init() {
  ...
  IDT_ENTRY3(80);
}
```

También declaro en `isr.h` la interrupción:

```h
void _isr80();
```

Por último, hay que definir la interrupción en asm:

```nasm
extern current_task
...
global _isr80
_isr80:
  pushad
  
  push [current_task]
  call sched_disable_task
  add esp, 4 
  call sched_next_task

  cmp ax, 0
  je .fin

  str bx
  cmp ax, bx
  je .fin

  mov word [sched_task_selector], ax

  jmp far [sched_task_offset]

  popad
  iret
```
```
```


b)

```nasm
extern current_task
...
global _isr80
_isr80:
  pushad
  
  push [current_task]
  call sched_disable_task
  add esp, 4 
  call sched_next_task ; devuelve en ax el selector de
  la próxima tarea a ejecutar

  push eax
  call pisar_EAX
  add esp, 4

  cmp ax, 0
  je .fin

  str bx
  cmp ax, bx
  je .fin

  mov word [sched_task_selector], ax

  jmp far [sched_task_offset]

  popad
  iret
```
```
```


```c
// en realidad los parámetros van al revés
void pisar_eax(uint16_t segsel) {
  tss_t* tss = obtenerTSS(segsel);
  uint32_t* pila = tss.esp;

  pila[7] = current_task;

}
```
```
```


c) 

```nasm
global hubo_exit
global id_tarea_exit

hubo_exit: dd 0
id_tarea_exit: dd -1

global _isr32
_isr32:
    pushad
    ; 1. Le decimos al PIC que vamos a atender la interrupción
    call pic_finish1
    call next_clock

    ; 2. Realizamos el cambio de tareas en caso de ser necesario
    call sched_next_task

    cmp ax, 0
    je .fin

    str bx
    cmp ax, bx
    je .fin

    cmp [hubo_exit], 0
    je .ejecucion_normal

    push [id_tarea_exit]
    push eax ; siguiente tarea
    call modificar_eax_2
    add esp, 8

    ; apagamos la flag hubo exit:
    mov [hubo_exit], 0

    .ejecucion_normal:
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
Y ahora modificamos la interrupción exit:
```nasm
...
global _isr80
_isr80:
  pushad
  
  push [current_task]
  call sched_disable_task
  add esp, 4 
  call sched_next_task

  cmp ax, 0
  je .fin

  str bx
  cmp ax, bx
  je .fin
; modificamos las variables globales para que se correspondan con la llamada a exit
  mov edi, 1
  mov dword [hubo_exit], edi
  mov edi, [current_task]
  mov dword [id_tarea_exit], edi

  mov word [sched_task_selector], ax

  jmp far [sched_task_offset]
 
.fin:
  popad
  iret
```
```
```

```c
void modificar_eax_2(uint8_t task_id, uint16_t segsel) {
  tss_t* tss = obtenerTSS(segsel);
  uint32_t pila = tss->esp;
  pila[7] = id_tarea;
}
```

d) No, no es una buena práctica. Problemas que pueden surgir son: que la tarea a la que se le sobrescribe el eax realmente lo necesite para ejecutarse correctamente. Además, la tarea a la que se le sobrescribe el EAX no es consciente de este hecho, por lo tanto no puede hacer mucho con eso. Una mejor forma de hacer esto podría ser escribir una variable en la memoria on demand, para que todas puedan acceder y escribirla.
