# Primer parcial - Arquitectura y Organización del Computador - Comision Furfi
### Segundo cuatrimestre 2025

- [Normas generales y modalidad de entrega](#normas-generales-y-modalidad-de-entrega)
	- [Régimen de Aprobación](#régimen-de-aprobación)
	- [Conocimientos a evaluar](#conocimientos-a-evaluar)
	- [Compilación y Testeo](#compilación-y-testeo)
	- [Verificación de tests](#verificación-de-tests)
- [Enunciado](#enunciado)
- [Estructuras](#estructuras)
	- [Ejercicio 1](#ejercicio-1)
	- [Ejercicio 2](#ejercicio-2)
   	- [Ejercicio 3](#ejercicio-3)



# Normas generales y modalidad de entrega

- El parcial es **INDIVIDUAL**.
- Una vez terminada la evaluación se deberá crear un PR al `main` de su repo desde la branch con la resolución como source.

> [!CAUTION]
> Es importante que no modifiquen los archivos de los tester. Si lo hacen, se nos advertirá y tendremos que desaprobar la entrega automáticamente.
> 
> **No se deben modificar manualmente:**
> - las carpetas test_utils y .github
> - los Makefiles
> - los test.c (salvo para comentar algún test y después dejarlo como estaba originalmente)

## Régimen de Aprobación

- El parcial es en los laboratorios, usando las compus de los labos o sus propias compus.
- Es a libro abierto, pueden tener todo lo que se les ocurra a disposición. Está totalmente prohibido el uso del celular, internet y chatbots.
- Deberán conectarse a un proxy que limitará su acceso a internet para que únicamente se puedan conectar a GitHub. 
- Les proveeremos de un script que deberá ejecutarse al comienzo del parcial y no detenerse hasta haber entregado. Con él nos aseguramos que permanezcan conectados al proxy.
- **Sólo se evalúa programación en Assembly**, **C no se corrige** pero recomendamos que primero planteen una solución en C y después pasarla a Assembly cuando tengan en claro cómo estructurar la solución.
- Las funciones auxiliares que implementen también deben estar en Assembly.
- Para aprobar el parcial deben implementar 2 de los 3 puntos del enunciado y que corran con éxito los tests funcionales, de abi y de memoria (valgrind).
- Vamos a usar herramientas de detección de plagio para asegurarnos de que su entrega sea original.

> [!NOTE]
> El comando `./tester.sh` corre los mismos tests que el juez online. Pueden usarlo para revisar que su parcial vaya a pasar los tests en dicho entorno.

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

Para compilar y ejecutar los tests en cada ejercicio se dispone de un archivo
`Makefile`, con los siguientes *targets*:

| Comando             | Descripción                                                         |
| ------------------- | ------------------------------------------------------------------- |
| `make test_c`       | Genera el ejecutable usando la implementación en C del ejercicio.   |
| `make test_asm`     | Genera el ejecutable usando la implementación en ASM del ejercicio. |
| `make test_abi`     | Genera el ejecutable usando la implementación en ASM del ejercicio + archivos necesarios para ABI enforcer. |
| `make run_c`        | Corre los tests usando la implementación en C.                      |
| `make run_asm`      | Corre los tests usando la implementación en ASM.                    |
| `make run_abi`      | Corre los tests usando la implementación en ASM + ABI enforcer.     |
| `make valgrind_c`   | Corre los tests en valgrind usando la implementación en C.          |
| `make valgrind_asm` | Corre los tests en valgrind usando la implementación en ASM.        |
| `make valgrind_abi` | Corre los tests en valgrind usando la implementación en ASM + ABI enforcer.        |
| `make check_offsets` | Corre el chequeo de offsets.                                         |
| `make run_main_c`        | Corre main usando la implementación en C.                      |
| `make run_main_asm`      | Corre main usando la implementación en ASM (no corre ABI enforcer).                    |
| `make clean`        | Borra todo archivo generado por el `Makefile`.                      |


## Verificación de tests
Para el parcial, contamos con una máquina de la facultad para correr los tests en un entorno limpio y controlado. La idea es que ustedes trabajen en su branch, haciendo los commits y push que necesiten, hasta tener los tests pasando en su computadora local.

> [!CAUTION]
> Para considerar un ejercicio aprobado, debe pasar los tests **con el comando** `make valgrind_abi`.

Una vez que tengan el parcial para entregar, procedan a revisar su aprobación del siguiente modo:
- Crean un PR de `su branch` a `main` con los cambios a entregar.
- **PARA CORRER LOS TESTS, DEBEN AGREGAR la ETIQUETA (label) "tests" AL PR**, verán en su repositorio que aparece una label `tests` a tal fin.
- Cuando se detecte que agregaron la label, denle uno o dos minutos (puede tardar más si hay muchas personas entregando al mismo tiempo) y se comenzarán a correr los tests. El estado de los mismos se informará en la ventana del PR donde figura el botón para hacer el merge.
- Eventualmente terminarán los tests y dirá si pasaron (aprobado) o no. Pueden revisar el progreso si hacen click en el nombre de la corrida en curso.
- **No hacer el merge a main del PR**.

> [!NOTE]
> Si hacen cambios y quieren volver a correr los tests, deben SACAR LA ETIQUETA, guardar el cambio (pueden refrescar la pagina por ejemplo) Y VOLVER A AGREGAR LA ETIQUETA.

# Enunciado
Una poderosa red social de Finlandia nos contrató para mejorar su código. En la primer parte del proyecto, la idea es mejorar la performance actual de los algortimos, por lo cual nos pidieron que realicemos las implementación en ASM. Esta empresa se llama Tuiter. Dentro de la app hay usuarios, que pueden seguirse unos a otros. Cada uno puede publicar mensajes que serán visibles para todos sus seguidores.

Contamos con las siguientes estructuras:

## Estructuras

```C
typedef struct {
	char mensaje[140];
	uint16_t favoritos;
	uint16_t retuits;
	uint32_t id_autor;
} tuit_t;

typedef struct {
	publicacion_t* next;
	tuit_t* value;
} publicacion_t;

typedef struct {
	publicacion_t* first;
} feed_t;

typedef struct {
	feed_t* feed;
	usuario_t** seguidores;
	uint32_t cantSeguidores;
	usuario_t** seguidos;
	uint32_t cantSeguidos;
	usuario_t** bloqueados;
	uint32_t cantBloqueados;
	uint32_t id;
} usuario_t;

```
---
## Ejercicio 1

Una de las funcionalidades básicas es la de publicar tuits. Vamos a implementar una función 

```C
tuit_t* publicar(char* mensaje, usuario_t* usuario);
```

que, tomando un mensaje y un usuario, debe armar un tuit y agregarlo como publicación al principio de su propio feed y de los feeds de sus seguidores.

> [!NOTE]
> El tuit deberá ser uno solo pero las publicaciones son propias de cada feed.

> [!NOTE]
> Al crear el tuit, el mensaje debe ser clonado.

Consejo: Armar una funcion auxiliar que dado un tuit y un feed, cree una publicación con el tuit y la agregue al comienzo del feed.

---

## Ejercicio 2

Para mejorar la experiencia de los usuarios, queremos que puedan bloquear a los usuarios que les generen incomodidad. Para esto vamos a implementar la función 

```C
void bloquearUsuario(usuario_t* usuario, usuario_t* usuarioABloquear);
```

que, tomando dos usuarios, bloquee el acceso del segundo a los tuits del primero y viceversa. Para esto hay que:
- Agregar el usuario bloqueado al final del arreglo de usuarios bloqueados.
- Borrar todas las publicaciones del feed del usuario bloqueador que contengan tuits del usuario bloqueado. 
- Borrar todas las publicaciones del feed del usuario bloqueado que contengan tuits del usuario bloqueador.

> [!NOTE]
> Al borrar una publicación, procurar no eliminar el tuit.
 
> [!NOTE]
> Se puede asumir que el arreglo de usuarios bloqueados posee suficiente espacio libre para poder agregar elementos. 

Consejo: armar funcion auxiliar que tome una feed y un usuario, y borre todas las publicaciones del feed que tengan tuits del usuario pasado.

---

## Ejercicio 3

Para incentivar la interacción entre todos los usuarios de la red, queremos implementar una sección "Trending Topic". Para lograr esto, vamos a seleccionar los tuits sobresalientes de un usuario.

Para ello, vamos a implementar la función

```C
tuit_t** trendingTopic(usuario_t* usuario, uint8_t (*esTuitSobresaliente)(tuit_t*));
```

que recibe un usuario y una función que determina si un tuit es sobresaliente o no (devuelve 0 si no, 1 si sí). La función ```trendingTopic``` recorrerá el feed del usuario y devolverá un arreglo que contenga punteros a los tuits sobresalientes **hechos por el usuario**. El arreglo devuelto debe terminar con un elemento NULL para marcar el final, a menos que no haya tuits sobresalientes, en cuyo caso no se debe devolver un arreglo, sino un puntero a NULL.

> [!NOTE]
> Procurar no devolver tuits de otros usuarios.

> [!NOTE]
> No se deben copiar los tuits.

Consejo: armar una función auxiliar que cuente la cantidad de tuits sobresalientes del feed de un usuario.