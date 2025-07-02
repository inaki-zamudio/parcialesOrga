a) Los descriptores de TSS de las cinco tareas

b) Habría que hacer que _isr32 incremente el ecx de la tarea justo antes de hacer jmp far a ella

```nasm
global _isr32
isr32:
  pushad

  call pic_finish1
  call next_clock

  str ax
  push ax
  call mod_ecx
  add esp, 2

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
```
```


Y definimos `mod_ecx`:

```c
void mod_ecx(uint16_t segsel) {
  uint32_t* tss = obtenerTSS(segsel); // definida en otras resoluciones de parciales
  uint32_t* esp = tss->esp;
  esp[5]++;
}
```

b) Habría que agregar una entrada a la IDT, que sea de nivel 3 ya que queremos que la puedan llamar las tareas, y con el número de interrupción 80 ya que a las syscalls se les suele poner números por encima del 80.
También hace falta declarar la interrupción en `isr.h`. Y luego definir el handler en `isr.asm`, lo cual hago en (d).

d)

```nasm
global _isr80
isr80:
  pushad

  push ecx
  push edi
  call fuiLlamadaMasVeces
  mov [esp + 28], eax
  popad
  iret
```
```


```c
uint8_t fuiLlamadaMasVeces(uint32_t id, uint32_t utc) {
  uint16_t selector = sched_tasks[id].selector;
  uint32_t utc_sel = obtenerUTC(selector);
  return utc > utc_sel;
}

uint8_t obtenerUTC(uint16_t segsel) {
  return obtenerTSS(segsel)->esp[5]
}
```
```
```
