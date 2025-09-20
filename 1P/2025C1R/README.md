[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-22041afd0340ce965d47ae6ef1cefeee28c7c493a6346c4f15d667ab976d596c.svg)](https://classroom.github.com/a/w5X8cWBO)
# Recuperatorio del Primer Parcial - Arquitectura y Organización del Computador
### Primer cuatrimestre 2025

- [Normas generales y modalidad de entrega](#normas-generales-y-modalidad-de-entrega)
  - [Régimen de Aprobación](#régimen-de-aprobación)
  - [Formulario de entrega](#formulario-de-entrega)
  - [Conocimientos a evaluar](#conocimientos-a-evaluar)
  - [Compilación y Testeo](#compilación-y-testeo)
  - [Verificación de tests](#verificación-de-tests)
- [Enunciado](#enunciado)
  - [Ejercicio 0 - Offsets](#ejercicio-0---offsets)
  - [Ejercicio 1 - `hay_accion_que_toque`](#ejercicio-1---hay_accion_que_toque)
  - [Ejercicio 2 - `invocar_acciones`](#ejercicio-2---invocar_acciones)
  - [Ejercicio 3 - `contar_cartas`](#ejercicio-3---contar_cartas)

# Normas generales y modalidad de entrega

- El parcial es **INDIVIDUAL**
- Una vez terminada la evaluación se deberá crear un PR a `main` con la branch con la resolución como source.
- Deben informar su entrega en el formulario de entrega, enviando el link al PR con sus cambios finales y los tests corridos (ver sección [Verificación de tests](#verificación-de-tests)):

> [!CAUTION]
> Es importante que no modifiquen los archivos de los tester, si lo hacen se nos advertirá y tendremos que desaprobar la entrega automáticamente.
> 
> **Sólo se pueden modificar:**
> - El archivo `solucion.c`
> - El archivo `solucion.asm`
> - Cualquier achivo nuevo que creen ustedes

## Formulario de entrega
https://docs.google.com/forms/d/e/1FAIpQLSfBCJdCNKPBy8CCOqRlUdtlMNZJmcHrXVxfHGpEJCn0VB4fsg/viewform

## Régimen de Aprobación

- El parcial es en los laboratorios, usando las compus de los labos o sus propias compus.
- Es a libro abierto, pueden tener todo lo que se les ocurra a disposición. Recomendamos evitar el uso de IA, en esta materia suele equivocarse y no es fácil encontrar errores en código no propio de C o ASM.
- **Sólo se evalúa programación en Assembly**. SIMD no entra en el parcial y **C no se corrije** pero recomendamos que primero planteen la solución en C y después pasarla a Assembly.
- Para aprobar el parcial deben estar resuelto el [ejercicio 0](#ejercicio-0---offsets) (offsets y tamaños de structs) y *dos de los otros tres*.
- Una solución correcta de un ejercicio de programación es aquella que *pasa los tests*, *está escrita en ASM*, *no tiene problemas de memoria (valgrind)* y *no tiene problemas de ABI*.
- Vamos a usar herramientas de detección de plagio para asegurarnos de que su entrega sea original. 

> [!NOTE]
> Los tests del juez online son equivalentes a correr `make valgrind_abi`. Pueden usar ese _target_ para revisar que su parcial vaya a pasar los tests en dicho entorno.

> [!NOTE]
> Durante el parcial estaremos disponibles para resolver consultas de enunciado y para destrabarles si están dando vueltas mucho tiempo en algo que no forma parte de lo evaluado. NO responderemos preguntas teóricas.


## Conocimientos a evaluar 

- Uso de memoria dinámica.
- Navegación de punteros.
- Representación binaria de los tipos de datos.
- Manejo de pila.
- Manejo de structs.
- Convención C, ABI, uso de registros.
- Uso del debugger GDB.

## Compilación y Testeo

Para compilar y ejecutar los tests cada ejercicio dispone de un archivo
`Makefile` con los siguientes *targets*:

| Comando              | Descripción                                                         |
| -------------------- | ------------------------------------------------------------------- |
| `make test_c`        | Genera el ejecutable usando la implementación en C del ejercicio.   |
| `make test_asm`      | Genera el ejecutable usando la implementación en ASM del ejercicio. |
| `make test_abi`      | Genera usando la implementación en ASM del ejercicio + archivos necesarios para ABI enforcer |
| `make run_c`         | Corre los tests usando la implementación en C.                      |
| `make run_asm`       | Corre los tests usando la implementación en ASM.                    |
| `make run_abi`       | Corre los tests usando la implementación en ASM + ABI enforcer.     |
| `make valgrind_c`    | Corre los tests en valgrind usando la implementación en C.          |
| `make valgrind_asm`  | Corre los tests en valgrind usando la implementación en ASM.        |
| `make valgrind_abi`  | Corre los tests en valgrind usando la implementación en ASM + ABI enforcer |
| `make clean`         | Borra todo archivo generado por el `Makefile`.                      |
| `make check_offsets` | Valida los offsets completados en `solucion.asm`                    |

Todos los ejercicios de este parcial se resuelven editando el mismo fichero.
A la hora de correr los tests **sólo se correrán los tests de los incisos que hayan marcado como hechos**.
Para marcar un ejercicio como hecho deben modificar la variable `EJERCICIO_1_HECHO`, `EJERCICIO_2_HECHO` ó `EJERCICIO_3_HECHO` según corresponda asignando `true` (en C) ó `TRUE` (en ASM).

## Verificación de tests

Para el parcial, contamos con una máquina de la facultad para correr los tests en un entorno limpio y controlado.
La idea es que ustedes trabajen en su branch, haciendo los commits y push que necesiten, hasta tener los tests pasando en su computadora local.

> [!CAUTION]
> Para considerar un ejercicio aprobado, debe pasar los tests **con el comando** `make valgrind_abi`

Una vez que tengan el parcial para entregar, proceden a revisar su aprobación del siguiente modo:
- Crean un PR de _su branch_ a `main` con los cambios a entregar
- **PARA CORRER LOS TESTS, DEBEN AGREGAR UNA ETIQUETA (label) AL PR**, verán en su repositorio que aparece una label `tests` a tal fin
- Cuando se detecte que agregaron la label, denle uno o dos minutos y comenzarán a correr los tests. El estado se informará en la ventana del PR donde figura el botón para hacer merge.
- Eventualmente terminarán los tests y dirá si pasaron (aprobado) o no. Pueden revisar el progreso si hacen click en el nombre de la corrida en curso.
- **No hacer el merge a main del PR**

> [!NOTE]
> Si hacen cambios y quieren volver a correr los tests, deben SACAR LA ETIQUETA, guardar el cambio (pueden refrescar la pagina por ejemplo) Y VOLVER A AGREGAR LA ETIQUETA

# Enunciado

La cátedra continúa trabajando en su juego de cartas Ah-Yi-Ok!, en el cual se enfrentan dos jugadores.
En este juego, cada jugador cuenta con una mano de cartas que irán colocando en un tablero de 10x5 espacios para activar las distintas acciones asociadas a las cartas.

```c
#define ANCHO_CAMPO 10
#define ALTO_CAMPO 5
```

Cada carta tiene un nombre, un dueño y una cantidad de puntos de vida.
Una carta puede estar en el campo de juego pero no estar en juego aún (se encuentra desactivada temporalmente o nunca se activó).

```c
typedef struct carta {
	bool en_juego;
	char nombre[12];
	uint16_t vida;
	uint8_t jugador;
} carta_t;
```

Participan del juego dos jugadores humanos: el jugador rojo y el jugador azul.
Además, el sistema del juego puede incorporar jugadores simulados (no-humanos) adicionales que también poseen la capacidad de colocar cartas en el tablero.

```c
#define JUGADOR_ROJO 1
#define JUGADOR_AZUL 2
```

Cada jugador cuenta con una "mano" de cartas de las cuales colocar las que desee en el campo de juego.
Los jugadores no-humanos **no** tienen una "mano".

```c
typedef struct tablero {
	carta_t* mano_jugador_rojo;
	carta_t* mano_jugador_azul;
	carta_t* campo[ALTO_CAMPO][ANCHO_CAMPO];
} tablero_t;
```

Los jugadores pueden decidir utilizar distintas acciones entre las de sus cartas en juego.
Cada acción tiene asociadas una **pieza de código**, una carta **destino** afectada por la acción, y una acción **siguiente** a ella.
Los punteros nulos se interpretan como la acción "fin del turno".

```c
typedef void accion_fn_t(tablero_t* tablero, carta_t* carta);

typedef struct accion {
	accion_fn_t* invocar;
	carta_t* destino;
	struct accion* siguiente;
} accion_t;
```

Nos interesa implementar tres funciones.

## Ejercicio 0 - Offsets

Completar los offets y tamaños de struct definidos en el archivo `solucion.asm`. 
Para validar este ejercicio pueden correr `make check_offsets`.
```nasm
carta.en_juego EQU NO_COMPLETADO
carta.nombre   EQU NO_COMPLETADO
carta.vida     EQU NO_COMPLETADO
carta.jugador  EQU NO_COMPLETADO
carta.SIZE     EQU NO_COMPLETADO

tablero.mano_jugador_rojo EQU NO_COMPLETADO
tablero.mano_jugador_azul EQU NO_COMPLETADO
tablero.campo             EQU NO_COMPLETADO
tablero.SIZE              EQU NO_COMPLETADO

accion.invocar   EQU NO_COMPLETADO
accion.destino   EQU NO_COMPLETADO
accion.siguiente EQU NO_COMPLETADO
accion.SIZE      EQU NO_COMPLETADO
```

- Las definiciones con el prefijo `carta` corresponden a la estructura `carta_t`
- Las definiciones con el prefijo `tablero` corresponden a la estructura `tablero_t`
- Las definiciones con el prefijo `accion` corresponden a la estructura `accion_t`
- Las definiciones con el sufijo `SIZE` corresponden a el tamaño de la estructura es cuestión
- Las definiciones que no poseen el sufijo `SIZE` corresponden al offset del campo en cuestión

## Ejercicio 1 - `hay_accion_que_toque`

Dada una secuencia de acciones determinar si hay alguna cuya carta tenga un nombre idéntico (mismos contenidos, no mismo puntero) al pasado por parámetro.
```c
bool hay_accion_que_toque(accion_t* accion, char* nombre);
```

El resultado es un valor booleano, la representación de los booleanos de C es la siguiente:
- El valor `0` es `false`
- Cualquier otro valor es `true`

## Ejercicio 2 - `invocar_acciones`

Dada una secuencia de acciones, invocarlas en orden en caso de que las reglas del juego lo permitan.
```c
void invocar_acciones(accion_t* accion, tablero_t* tablero);
```

Una acción debe ser invocada **sí y sólo sí** la carta a la que está destinada la acción se encuentra en juego.
Luego de invocarse una acción, su carta destino debe pasar a estar fuera de juego si sus puntos de vida son 0.

Las funciones que implementan acciones de juego tienen la siguiente firma:
```c
void mi_accion(tablero_t* tablero, carta_t* carta);
```
- El tablero a utilizar es el pasado como parámetro
- La carta a utilizar es la carta destino de la acción (`accion->destino`)

Las acciones se deben invocar en el orden natural de la secuencia (primero la primera acción, segundo la segunda acción, etc). Las acciones asumen este orden de ejecución.

Se deben tener en cuenta las siguientes consideraciones:
- Una carta con 0 puntos de vida puede estar en juego *antes* de invocarse una acción.
- Una carta puede pasar a estar fuera de juego por razones ajenas a sus puntos de vida (tal vez la acción la pone a dormir).
- Una carta que tiene cero puntos de vida *después* de invocarse su acción *sí o sí* pasa a estar fuera de juego (independientemente de si antes tenía 0 puntos o no).

## Ejercicio 3 - `contar_cartas`

Contar la cantidad de cartas en el tablero correspondientes a cada uno de los jugadores.

```c
void contar_cartas(tablero_t* tablero, uint32_t* cant_rojas, uint32_t* cant_azules);
```

Se deben tener en cuenta las siguientes consideraciones:
- Además del jugador rojo y el jugador azul puede haber cartas asociadas a otros jugadores simulados (no-humanos).
- Las posiciones libres del campo tienen punteros nulos en lugar de apuntar a una carta.
- El resultado debe ser escrito en las posiciones de memoria proporcionadas como parámetro.
- El conteo incluye tanto a las cartas en juego cómo a las fuera de juego (siempre que estén visibles en el campo).
