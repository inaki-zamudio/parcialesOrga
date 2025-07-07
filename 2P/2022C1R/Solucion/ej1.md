1)
Habría que definir una nueva entrada en la idt, en `idt.c`:

```c
idt_init() {
  IDT_ENTRY3(80); // nivel 3 ya que quiero que todas las tareas de usuario la puedan llamar
}
```

Luego, habría que declarar en `isr.h` el handler de la interrupción:

```h
...
void _isr80();
```

2)
- Crear una nueva tarea. Esto es, designar en la GDT una entrada para el TSS descriptor, agregar la tarea al scheduler, setearle los valores por defecto (de lo que se encarga `tss_create_user_task`), agregarla al scheduler. Lo descrito es exactamente lo que hace `create_task` de `tasks.c`.
- En la tss de la nueva tarea, vamos a querer copiar la TSS de la tarea que llamó a la syscall.
- Hay que crear el esquema de paginación para la nueva tarea, pero quitar el atributo de escritura. La función `tss_create_user_task` va a ponernos el mismo mapeo de todo excepto de stack de nivel 0.
