### Arquitectura y Organización de Computadoras - DC - UBA 
# Segundo Parcial - Primer Cuatrimestre de 2025

## Enunciado

En esta oportunidad, nos encargaron diseñar un sistema de asignación de memoria para nuestro kernel, que permita que cada tarea pueda pedir memoria de forma dinámica y liberarla en caso de que ya no la necesite. Como mecanismo de asignación de memoria, el kernel implementará un sistema de *lazy allocation* que permite que las tareas puedan pedir memoria, pero que el kernel no la reserve hasta que realmente se acceda a dicha memoria.

Contamos con la función `malloco` ya implementada en el kernel que recibe por parámetro la cantidad de memoria a reservar en bytes y devuelve la dirección virtual a partir de la cuál se reservó memoria (Pero aún no estará mapeada a direcciones físicas). Devolverá NULL en caso de que no haya memoria suficiente.

```C
void* malloco(size_t size);
```

Contamos con la función `esMemoriaReservada` que recibe una dirección virtual y retorna `true` si la misma pertenece a un bloque de memoria reservado por malloco, `false`si no.

```C
uint8_t esMemoriaReservada(virtaddr_t virt)
```

Contamos con la función `chau` que recibe una dirección virtual y, si corresponde al comienzo de un bloque reservado con malloco, marca esa reserva para que sea liberada más adelante por una tarea de nivel 0 (desarrollado más adelante). No nos preocuparemos por reciclar la memoria liberada, bastará con liberarla.

```C
void chau(virtaddr_t virt);
```

Para llevar registro de las reservas de memoria, el sistema cuenta con un array alojado estáticamente en la memoria del kernel. Los elementos del array serán  structs de la pinta:

```C
typedef struct {
  uint32_t task_id;
  reserva_t* array_reservas;
  uint32_t reservas_size;
} reservas_por_tarea; 
```

- task_id: identifica a qué tarea corresponden las reservas de este item
- array_reservas: array de reservas en donde una reserva es un struct reserva_t 
- reservas_size: cantidad de elementos de array_reservas
- No nos vamos a preocupar por el tamaño del array_reservas, asumiremos que tendrá espacio suficiente para satisfacer las necesidades del sistema.
  
```C
typedef struct {
  uint32_t virt; //direccion virtual donde comienza el bloque reservado
  uint32_t tamanio; //tamaño del bloque en bytes
  uint8_t estado; //0 si la casilla está libre, 1 si la reserva está activa, 2 si la reserva está marcada para liberar, 3 si la reserva ya fue liberada
} reserva_t; 
```

Contamos con la función `dameReservas` que recibe por parámetro un task_id y devuelve el puntero al elemento del array que corresponde a esa tarea.

```C
reservas_por_tarea* dameReservas(int task_id);
```

Como se implementará un sistema de *lazy allocation*, el kernel no asignará memoria física hasta que la tarea intente acceder, ya sea por escritura o lectura, a la memoria reservada. En el momento del acceso, si la dirección virtual corresponde a las reservadas por la tarea, el kernel deberá asignar memoria física a la dirección virtual que corresponda. La asignación es gradual, es decir, solamente se asignará una única página física por cada acceso a la memoria reservada. A medida que haya más accesos, se irán asignando más páginas físicas. Las páginas de memoria física son obtenidas del area libre de tareas, se asume que habrá suficientes. La memoria asignada por este mecanismo debe estar inicializada a cero (como cuando se reserva memoria con 'calloc'). Si el acceso es incorrecto porque la tarea está leyendo una dirección que no le corresponde, el kernel debe desalojar tarea inmediatamente y asegurarse de que no vuelva a correr, marcar la memoria reservada por la misma para que sea liberada y saltar a la próxima tarea.

La liberación de memoria va a estar a cargo de una tarea de nivel 0 llamada `garbage_collector` que recorrerá continuamente las reservas de las tareas en busca de reservas marcadas para liberar.

Ejercicio 1: (50 puntos)

Detallar todos los cambios que es necesario realizar sobre el kernel para que una tarea de nivel usuario pueda pedir memoria y liberarla, asumiendo como ya implementadas las funciones mencionadas. Para este ejercicio se puede asumir que `garbage_collector` está implementada y funcionando.

Ejercicio 2: (25 puntos)

Detallar todos los cambios que es necesario realizar sobre el kernel para incorporar la tarea `garbage_collector` si queremos que se ejecute una vez cada 100 ticks del reloj. Incluir una posible implementación del código de la tarea.

Ejercicio 3:

a)Indicar dónde podría estár definida la estructura que lleva registro de las reservas (5 puntos)

b)Dar una implementación para `malloco` (10 puntos)

Considerando:
- Como máximo, una tarea puede tener asignados hasta 4 MB de memoria total. Si intenta reservar más memoria, la syscall deberá devolver `NULL`.
- El área de memoria virtual reservable empieza en la dirección `0xA10C0000`
- Cada tarea puede pedir varias veces memoria, pero no puede reservar más de 4 MB en total.
- No hace falta contemplar los casos en que las reservas tienen tamaños que no son múltiplos de 4KB. Es decir, las reservas siempre van a ocupar una cantidad de páginas, y dos reservas distintas nunca comparten una misma página.

c)Dar una implementación para `chau` (10 puntos)

Considerando:
- Si se pasa un puntero que no fue asignado por la syscall `malloco`, el comportamiento de la syscall `chau` es indefinido.
- Si se pasa un puntero que ya fue liberado, la syscall `chau` no hará nada.
- Si se pasa un puntero que pertenece a un bloque reservado pero no es la dirección más baja, el comportamiento de la syscall `chau` es indefinido.
- Si la tarea continúa usando la memoria una vez liberada, el comportamiento del sistema es indefinido.
- No nos preocuparemos por reciclar la memoria liberada, bastará con liberarla