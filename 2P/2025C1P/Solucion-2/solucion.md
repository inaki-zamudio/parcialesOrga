Vamos a definir una nueva syscall `void* malloco(size_t size)`, que reserva memoria de forma dinámica. Si no hay memoria disponible, devolverá NULL.

Primero, en isr.h, agregamos la declaración de la syscall:
```c
...
void _isr80();
```

Luego, en idt.c, agregamos a la función idt_init() lo siguiente:

```c
    ...
    IDT_ENTRY3(80);
```
Esto es porque queremos que los programas del espacio de usuario puedan invocar a dicha interrupción.
En isr.asm, definimos la rutina de atención para la nueva interrupción. Asumo que recibo, por EAX, el parámetro `size` necesario:

```nasm
    ...
    extern malloco
    ...
    global _isr80
    
    _isr80:
        pushad
        push eax
        call malloco
        add esp, 4
        popad
        iret
``` 
Antes que nada, defino el arreglo que me pide el enunciado, donde cada elemento representa una reserva:
```c
    typedef struct {
        uint8_t task_id;
        uint32_t tamaño;
        uint32_t inicio;
    } reserva_t;

    reserva_t malloco_arr[TAMAÑO_NECESARIO];
    uint32_t ULT_ACCESO = -1; // variable global que indica ùltimo accedido en malloco_arr
```
Ahora escribimos el código en C de malloco:

```c
    #define 4_MB_EN_BYTES ...

    void* malloco(size_t size) {
        if (reservo_mas_de_4_mb(current_task, size)) return NULL;
        malloco_arr[ULT_ACCESO++] = { // incremento ULT_ACCESO
            task_id = current_task; // variable global current_task
            tamaño = size; 
            inicio = inicio_prox_bloque();
        }
        START_DYN_MEM_PHYSICAL += size;
        return malloco_arr[ULT_ACCESO]; 
    }

    bool reservo_mas_de_4mb(uint8_t task_id, size_t tamaño_nueva_reserva) {
        uint32_t total = size;
        for (size_t i = 0; i <= ULT_ACCESO; i++) {
            if (malloco_arr[i].task_id == task_id) total += malloco_arr[i].size; 
        }
        return total > 4_MB_EN_BYTES;
    }
    
    uint32_t inicio_prox_bloque(uint8_t task_id) {
        uint32_t total = 0;
        for (size_t i = 0; i <= ULT_ACCESO; i++) {
            if (malloco_arr[i].task_id == task_id) {
                total += malloco_arr[i].size; 
            } 
        }
        return total;
    }
```

Por ahora, no nos estuvimos preocupando del caso en el que la tarea efectivamente quiera acceder a la memoria que pidió. Para eso, vamos a tener que modificar el page fault handler, para que si la tarea quiere acceder a una página que pidió, hagamos la asignación efectiva.

```nasm
    global _isr14
    
    _isr14:
        pushad
        mov ecx, cr2
```
